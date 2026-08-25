Compilar:

    make clean && make

Executar:

    ./processflow

testes:

task listar /bin/ls
    task ordenar /usr/bin/sort
    task contar /usr/bin/wc -l
    run pipe listar ordenar contar
    task ola /bin/echo ola
    task mundo /bin/echo mundo
    run sequential ola mundo
    run parallel ola mundo
    exit 