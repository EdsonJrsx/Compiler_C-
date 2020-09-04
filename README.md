Lucas Fernando Alvarenga e Silva    RA: 120857
Edson Junior Fontes Carvalho        RA: 120362

Para compilar o projeto é necessário rodar os seguintes comandos em ordem:
bison -d parser.y
flex scanner.l
gcc -c *.c
gcc -o exec *.o -ly -lfl

o executavel exec deve receber dois parametros, sendo eles:
 --> o primeiro parametro é referente ao caminho para o arquivo fonte
 --> o segundo parametro é opcional e é referente ao caminho para o arquivo objeto (não será utilizado nesta versão).

 OBS.:  Caso queira não imprimir a arvore sintática --> Comentar a linha 37, "printTree()".
        Caso queira não imprimir a tabela de símbolos --> enviar 0 para a função buildSymtab(..., 0).