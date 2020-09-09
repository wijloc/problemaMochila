#include <vector>
#include <iostream>
#include <numeric>
#include <ostream>
#include <algorithm>
#include <tuple>

using namespace std;

int genId = 1;

class Item{
  int id;
  int tamanho;
  int valor;
  public:
    Item (int id, int tamanho, int valor){
      this->id = id;
      this->tamanho = tamanho;
      this->valor = valor;
    }

    const int getValor(){
      return this->valor;
    }

    const int getTamanho(){
      return this->tamanho;
    }

};

class Solucao{  

  public: 
    vector<int> escolhidos;
    vector<Item> itens;
    int tamanhoMochila;
    int id;

    Solucao(vector<Item> itens, int tamanhoMochila){
      this->escolhidos = vector<int>(itens.size(), 0);
      this->itens = itens;
      this->tamanhoMochila = tamanhoMochila;
      this->id = genId++;
    }

    Solucao copy(){
      Solucao aux = Solucao(this->itens, this->tamanhoMochila);
      aux.escolhidos = this->escolhidos;
      return aux;
    }

    int fitness(){
      int valor = 0;
      int tamanho = 0;
      for (int i = 0; i < itens.size(); i++){
        valor += escolhidos[i] * itens[i].getValor();
        tamanho+= escolhidos[i] * itens[i].getTamanho();
      }

      if (tamanho > tamanhoMochila)
        return 0;
      else
        return valor;
    }

    static Solucao solucaoAleatoria(vector<Item> itens, int tamanhoMochila){      
      while(true){
        Solucao s(itens,tamanhoMochila);
        for(int i = 0; i < s.itens.size(); i++){
          s.escolhidos[i] = rand() % 2;
        }
        if (s.fitness() > 0)
          return s;
      }
    }

    void print(){
      for(int i = 0; i < itens.size(); i++){
        cout << escolhidos[i] << "|";
      }
      cout << endl;
    }
};

class PM{
  int n;
  int W;
  vector<Item> itens;
  
  public:
    PM(){};

    PM(int n, int W){
      this->n = n;
      this->W = W;
    };

    int getNumeroItens(){
      return n;
    }

    int getTamanhoMochila(){
      return W;
    }

    vector<Item> getItens(){
      return itens;
    }

    void incluirItem(int id, int tamanho, int valor){
      itens.push_back(Item(id, tamanho, valor));
    }    
};

class AlgoritmoGenetico{
  
  vector<Solucao> populacao;
  PM pm;
  int TAMANHO_POPULACAO_INICIAL;
  public:

  AlgoritmoGenetico(PM pm, int tamanhoPopulacaoInicial){
    this->pm = pm;
    this->TAMANHO_POPULACAO_INICIAL = tamanhoPopulacaoInicial;
  }

  Solucao resolver(){
    for(int i = 0; i < this->TAMANHO_POPULACAO_INICIAL; i++){
      populacao.push_back(Solucao::solucaoAleatoria(pm.getItens(), pm.getTamanhoMochila()));
    }    
    
    sort(populacao.begin(), populacao.end(), [](Solucao a, Solucao b){return a.fitness() > b.fitness(); });
    cout << "Best Fitness da população inicial: " << populacao[0].fitness() << endl;

    int geracao = 0;
    while (geracao < 10){

      vector<Solucao> reprodutores = populacao;
      double taxaReproducao = 1;
      int iteracoes = ((taxaReproducao * populacao.size())/2);
      int i = 0;
      while(i < iteracoes){
        Solucao a = selecaoRoleta(reprodutores);
        Solucao b = selecaoRoleta(reprodutores);
        reproducao(a, b, populacao);
        i++;
      }

      sort(populacao.begin(), populacao.end(), [](Solucao a, Solucao b){return a.fitness() > b.fitness(); });
      cout << "Best Fitness após reprodução população inicial: " << populacao[0].fitness() << endl;

      mutacao(populacao);

      ajustePopulacao(populacao);
      geracao++;
    }

    return populacao[0];
  }

  void ajustePopulacao(vector<Solucao> &populacao){
    sort(populacao.begin(), populacao.end(), [](Solucao a, Solucao b){return a.fitness() > b.fitness(); });
    while(populacao.size() > TAMANHO_POPULACAO_INICIAL){
      populacao.pop_back();
    }
  }

  void mutacao(vector<Solucao> &populacao){
    for(int i = 0; i < populacao.size(); i++){
      if (((double) rand() / (RAND_MAX)) < 0.05){
        int gene = rand() % populacao[i].itens.size();
        populacao[i].escolhidos[gene] = populacao[i].escolhidos[gene]==1?0:1;
      }
    }
  }

  void reproducao(Solucao a, Solucao b, vector<Solucao> &populacao){
    Solucao filho1 = a.copy();
    Solucao filho2 = b.copy();
    int point = (rand() % (a.itens.size() - 1)) + 1;
    for(int i = 0; i < point; i++){
      filho1.escolhidos[i] = b.escolhidos[i];
    }

    for(int i = point + 1; i < a.itens.size(); i++){
      filho2.escolhidos[i] = a.escolhidos[i];
    }

    populacao.push_back(filho1);    
    populacao.push_back(filho2);

    cout << a.fitness() << " + " << b.fitness() << " = " << filho1.fitness() << " " << filho2.fitness() << endl;
  }

  Solucao selecaoRoleta(vector<Solucao> &solucoes){
    vector<tuple<Solucao, double>> roleta;
    for(Solucao s : solucoes){
      roleta.push_back(make_tuple(s,s.fitness()));
    }

    double totalFitness = accumulate(roleta.begin(), roleta.end(), 0.0, [](double result, const tuple<Solucao, double> itemRoleta){ return result + get<double>(itemRoleta);});

    double hit = ((double) rand() / (RAND_MAX));

    double sum = 0;
    for(tuple<Solucao, double> itemRoleta : roleta){
      sum += get<double>(itemRoleta) / totalFitness;
      if (hit < sum){
        solucoes.erase(remove_if(solucoes.begin(), solucoes.end(), [a = get<Solucao>(itemRoleta)](Solucao s){return (s.id == a.id);}),
        solucoes.end());
        return get<Solucao>(itemRoleta);
      }
    }
  }
};

int main(){
  srand(time(NULL));

  //Instanciando o problema
  PM pm = PM(5, 80);
  pm.incluirItem(1, 10, 15);
  pm.incluirItem(2, 40, 90);
  pm.incluirItem(3, 26, 50);
  pm.incluirItem(4, 32, 60);
  pm.incluirItem(5, 8, 12);
  pm.incluirItem(6, 6, 11);

  AlgoritmoGenetico ag(pm, 10);

  Solucao s = ag.resolver();
  s.print();
  cout << s.fitness() << endl;
}