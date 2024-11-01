// C++
//
// Jogo Whack-a-Mole com um Arduino Uno
// Concebido por: Carlos Oliveira
//

// Define o pino do botão e inicializa variáveis para armazenar estados e tempos que serão utilizados ao longo do programa:
const byte botao= 4; // Pino onde o botão está conectado.
byte estado_botao;
byte ultimo_estado_botao= HIGH; // Último estado do botão (inicialmente HIGH pois o botão está configurado como INPUT_PULLUP).
unsigned long ultimo_tempo_debounce= 0;
const byte tempo_debounce= 20; // Tempo de debounce em milissegundos.
const int tempo_reset= 2000; // Tempo necessário para iniciar o jogo ao pressionar o botão.
const byte tempo_acender_led= 500; // Tempo em milissegundos em que o LED aleatório fica aceso.
byte leitura_estado_botao;
byte led_atual= 0b00000001; // Variável responsável por armazenar o estado atual do LED que está aceso.
byte leds_acesos= 0b00000000; // Variável responsável por armazenar o estado de todos os LEDs que estão acesos.

void setup(){
    Serial.begin(9600); // Inicializa a comunicação Serial com um rate de 9600 bps para debug.
    randomSeed(analogRead(0)); // Inicializa o gerador de números aleatórios com um valor aleatório.
    pinMode(botao, INPUT_PULLUP); // Configura o pino do botão como sendo de entrada e Pull-Up.
    for (byte led= 7; led<= 13; led++){
        pinMode(led, OUTPUT); // Configura os pinos dos LEDs como saída.
    }
}

void loop(){
    leitura_estado_botao=digitalRead(botao); // Lê o estado atual do botão.
    if (leitura_estado_botao!=ultimo_estado_botao){
        ultimo_tempo_debounce=millis(); // Atualiza o tempo de debounce.
    }

    if ((millis()-ultimo_tempo_debounce)>tempo_debounce){
        if (leitura_estado_botao!=estado_botao){
            estado_botao= leitura_estado_botao; // Atualiza o estado do botão.
            if (estado_botao==LOW){
                unsigned long tempo_inicio_pressao= millis(); // Marca o tempo de início da pressão do botão.
                while (digitalRead(botao)==LOW){
                    if ((millis()-tempo_inicio_pressao)>=tempo_reset){
                        Serial.println("--- !JOGO INICIADO! ---"); // Imprime uma mensagem de início do jogo na comunicação Serial para debug.
                        animacao_leds(7, 13, 3); // Animação de início do jogo.
                        jogo_whack_a_mole(); // Chama a função para iniciar o jogo.
                        break; // Sai do loop quando o jogo é finalizado e retorna ao estado IDLE do programa.
                    }
                }
            }
        }
    }
    ultimo_estado_botao= leitura_estado_botao; // Atualiza o último estado do botão.
}

// Função responsável por fazer as animações de Início, Fim e Reset do jogo (NOTA: Esta função, ao contrário da outra, pode ser utilizada fora deste código.):
void animacao_leds(byte LED_INICIAL, byte LED_FINAL, byte NUMERO_REPETICOES) {
    for (int cont= 1; cont<= NUMERO_REPETICOES; cont++){
        for (int led= LED_INICIAL; led<= LED_FINAL; led++){
            digitalWrite(led, HIGH);
        }
        delay(150);
        for (int led= LED_INICIAL; led<= LED_FINAL; led++){
            digitalWrite(led, LOW);
        }
        delay(150);
    }
    return; // Este return serve apenas para marcar o fim da função.
}

// Função responsável por executar o código do jogo Whack-a-Mole (NOTA: Esta função serve apenas para manter o código mais limpo e organizado e não pode ser utilizada fora deste código uma vez que faz recurso de variáveis globais e de outras funções presentes apenas neste código.):
void jogo_whack_a_mole(){

    // Loop principal do jogo:
    while (true){

        // Gera um número aleatório, entre 7 e 13 para acender o LED ligado ao Pin com o número correspondente:
        byte led_aleatorio= random(7, 14);
        digitalWrite(led_aleatorio, HIGH);

        // Inicia um contador para auxiliar na leitura do botão e define o estado do botão como não pressionado:
        unsigned long tempo_inicio= millis();

        // Loop para verificar se o botão foi pressionado durante o tempo em que o LED aleatório está aceso (contém um método para debounce):
        while ((millis()-tempo_inicio)<tempo_acender_led){
            leitura_estado_botao= digitalRead(botao);
            if (leitura_estado_botao!= ultimo_estado_botao){
                ultimo_tempo_debounce= millis();
            }

            if ((millis()-ultimo_tempo_debounce)>tempo_debounce){
                if (leitura_estado_botao!=estado_botao){
                    estado_botao=leitura_estado_botao;

                    // Condição para na eventualidade do botão ser pressionado durante o tempo em um LED que esteja aceso, efetua as operações Bitwise responsáveis por ligar e desligar os LEDs do jogo e sai loop de verificação:
                    if (estado_botao==LOW){
                        // Operações Bitwise e comentários para debug:
                        Serial.println("------------ !Botao primido no tempo correto! ------------");
                        Serial.print("Numero aleatorio gerado: ");
                        Serial.println(led_aleatorio);
                        led_atual= led_atual<<(led_aleatorio-6); // Desloca o bit para a posição do LED aleatório.
                        leds_acesos= led_atual|leds_acesos; // Operação Bitwise OR para ligar o LED aleatório, preservando o estado dos outros LEDs.
                        Serial.print("Led ligado pelo numero aleatorio gerado: ");
                        Serial.println(led_atual, BIN);
                        Serial.print("Leds acesos: ");
                        Serial.println(leds_acesos, BIN);
                        Serial.println("----------------------------------------------------------");

                        break;
                    }
                }
            }

            // Condição para verificar se o botão foi primido durante o tempo definido para RESET:
            if ((millis()-ultimo_tempo_debounce)>tempo_reset){
                if (leitura_estado_botao==estado_botao && estado_botao==LOW){
                // Sequência de reset:
                Serial.println("--- !JOGO RESETADO! ---"); // Imprime uma mensagem de reinício do jogo na comunicação Serial para debug.
                leds_acesos= 0b00000000;
                for (byte led= 7; led<= 13; led++){
                    digitalWrite(led, LOW);
                }
                animacao_leds(7, 13, 2);
                return;
                }
            }

            ultimo_estado_botao= leitura_estado_botao;

        }

        // Condição para manter o LED aceso até ao fim do jogo caso o botão tenha sido pressionado durante o tempo definido:
        for (byte led= 7; led<= 13; led++){
            if (leds_acesos & (1<<(led-6))){
                digitalWrite(led, HIGH);
            } else {
                digitalWrite(led, LOW);
            }
        }

        // Condição para verificar se todos os LEDs estão acesos e finalizar o jogo:
        if (0b11111110==leds_acesos){
            leds_acesos= 0b00000000;
            Serial.println("--- !JOGO TERMINADO! ---"); // Imprime uma mensagem de fim do jogo na comunicação Serial para debug.
            for (int led= 7; led<= 13; led++){
                digitalWrite(led, LOW);
                }
                delay(150);
                animacao_leds(7, 13, 3);
                return;
        }

        led_atual= 0b00000001; // Esta condição é responsável por reeiniciar a variável de led atual sempre que um ciclo do jogo é completado.

    }
}


// -- Observações Importantes: 
// 1. O código acima foi concebido para ser utilizado com um Arduino Uno e foi testado com sucesso. No entanto, é possível que seja necessário fazer ajustes para que funcione corretamente com outros modelos de Arduino ou com outros microcontroladores.
// 2. O código acima foi concebido para ser corrido sem interrupções num prazo máximo de aproximadamente 49 dias devido à natureza do tipo de variável utilizada para armazenar o tempo e à maneira como o tempo é contabilizado. Caso seja necessário correr o código por um período de tempo superior é necessário fazer ajustes para evitar overflow na variável que armazena o tempo e na função que é responsável por o contabilizar (função millis).
