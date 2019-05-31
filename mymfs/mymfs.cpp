// mymfs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "comandos.h"
#include <iostream>
#include <string>
#include <filesystem>

namespace fsys = std::experimental::filesystem;
using namespace std;

int main(int argc, char **argv) {
	
	//Deve possuir no minimo 3 argumentos (nome do programa - passado automaticamente,
	//caminho da unidade X, comando a ser executado)
	if (argc >= 3) {
		string caminhoComando = argv[1]; //Caminho de onde o Mymfs deve ser executado
		string comando = argv[2];        //Comando do Mymfs que deve ser executado

		if (comando == "config") {
			config(caminhoComando);
		}

		else if (comando == "import") {
			string caminhoArquivoImport = argv[3];    //Caminho do arquivo a ser importado para o diretório especficiado
			importarArquivo(caminhoComando, caminhoArquivoImport);
		}

		else if (comando == "export") {
			string caminhoArquivoExport = argv[3];    //Caminho do arquivo a ser exportado para o diretório especficiado
			string caminhoDiretorioExport = argv[4];  //Caminho do diretório para onde o arquivo deve ser exportado

			exportarArquivo(caminhoComando, caminhoArquivoExport, caminhoDiretorioExport);
		}
		else if (comando == "listall") {
			listAll(caminhoComando);
		}
	}
	else {
		cout << "Por favor, informe os argumentos necessarios" << endl;
	}

	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
