# EP 1 MVGA
EP 1 de MVGA da matéria do professor Helton, que pede para utilizar coordenadas baricênticas para achar o menor caminho até um triângulo

# Como funciona o programa

Temos um mapa do Brasl, providenciado pelo professor Helton, com o clique esquerdo do mouse, vc traça o caminho mais curto (se existir) de São Paulo (id = 170) até o ponto clicado, assim pintando o triângulo ou pintando a borda se não existir um caminho sem bordas, e também define o ponto de chegada do clique direito do mouse. O clique direito do mouse faz com que o caminho saia do triângulo clicado com o clique direito e vá até o triângulo clicado com o clique esquerdo previamente, pintando a borda da mesma maneira se bater nela.

# Explicação das funções

    void determinaInicio()

        - Determina o id_init para o id do triângulo no clique esquerdo

    bool baricêntrica(int &menor, double xp, double yp, int id, double &bar1, double &bar2, double &bar3)

        - Calcula todas as coordenadas baricêntricas:
            Fatos:
                p = bar1*a + bar2*b + bar3*c
                bar1 + bar2 + bar3 = 1
            
            Contruir um sistema de 3 equações e 3 incógnitas:
                bar1*xa + bar2*xb + bar3*xc = xp
                bar1*ya + bar2*yb + bar3*yc = yp
                bar1    + bar2    + bar3    = 1
            Os valores dos determinantes podem ser obtidos pela regra de Cramer, e sao 
            equivalentes as seguintes areas dos triangulos:
                Det = areaABC <-> D
                bar1 = areaPBC/areaABC <-> D1/D
                bar2 = areaAPC/areaABC <-> D2/D
                bar3 = areaABP/areaABC <-> D3/D
            Evidentemente a soma D1+D2+D3 = D.
            Cálculos das áreas dos triangulos
                    areaABC = 0.5*((xa*yb)-(ya*xb)+(ya*xc)-(xa*yc)+(xb*yc)-(yb*xc))
                    areaPBC = 0.5*((xp*yb)-(yp*xb)+(yp*xc)-(xp*yc)+(xb*yc)-(yb*xc))
                    areaAPC = 0.5*((xa*yp)-(ya*xp)+(ya*xc)-(xa*yc)+(xp*yc)-(yp*xc))
                    areaABP = 0.5*((xa*yb)-(ya*xb)+(ya*xp)-(xa*yp)+(xb*yp)-(yb*xp))

        - Verifica se as coordenadas estão dentro de um triângulo:
            Definicao:
                    Se as coordenadas baricentricas forem todas positivas, o ponto pertence ao triangulo ABC:
                        if (bar1 > 0 && bar2 > 0 && bar3 > 0) { return true; }
                    Senão, o ponto está na direção oposta à coordenada mais negativa.
                    Ou seja, primeiramente devemos verificar qual das coordenadas é a mais negativa:
                        if (bar1 < bar2 && bar1 < bar3) { menor = bar1; }
                        if (bar2 < bar1 && bar2 < bar3) { menor = bar2; }
                        if (bar3 < bar1 && bar3 < bar2) { menor = bar3; }

    void solucaoEP(double xp, double yp)

        - Pega a função baricentrica e traça o menor caminho dos triângulos

EP 1