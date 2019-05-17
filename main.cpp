#include "comandos.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv){

    //Deve possuir no minimo 3 argumentos (nome do programa - passado automaticamente,
    //caminho da unidade X, comando a ser executado)
    if(argc >= 3){
        string caminhoComando = argv[1]; //Caminho de onde o Mymfs deve ser executado
        string comando = argv[2];        //Comando do Mymfs que deve ser executado

        if(comando == "config"){
            config(caminhoComando);
        }

        else if(comando == "import"){
            string caminhoArquivoImport = argv[3];    //Caminho do arquivo a ser importado para o diretório especficiado

            importarArquivo(caminhoComando, caminhoArquivoImport);
        }

        else if(comando == "export"){
            string caminhoArquivoExport = argv[3];    //Caminho do arquivo a ser exportado para o diretório especficiado
            string caminhoDiretorioExport = argv[4];  //Caminho do diretório para onde o arquivo deve ser exportado

            exportarArquivo(caminhoComando, caminhoArquivoExport, caminhoDiretorioExport);
        }
        else if(comando == "listall"){
            listAll(caminhoComando);
        }
    }
    else{
        cout << "Por favor, informe os argumentos necessarios" << endl;
    }

    return 0;
}
