#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ctype.h>

constexpr uint8_t PCA9685_ENDERECO = 0x40;
// Celula Braille completa: canais 0 a 5 representam os pontos 1 a 6.
constexpr uint8_t QUANTIDADE_SERVOS = 6;
constexpr int ANGULO_REPOUSO = 0;
constexpr int ANGULO_PONTO_ATIVO = 90;
constexpr uint32_t TEMPO_EXIBICAO_MS = 20000;
// Faixa conservadora para SG90 a 50 Hz: aproximadamente 0,59 ms a 2,44 ms.
// Valores extremos demais podem ser ignorados pelo servo ou forcar o batente.
constexpr uint16_t SERVOMIN = 120;
constexpr uint16_t SERVOMAX = 500;
constexpr uint8_t PINO_SDA = D4;
constexpr uint8_t PINO_SCL = D5;

Adafruit_PWMServoDriver pca9685(PCA9685_ENDERECO);
bool pcaConectado = false;
uint8_t sdaEmUso = PINO_SDA;
uint8_t sclEmUso = PINO_SCL;

// Cada comando so e processado depois de Enter.
char comandoSerial[8] = {};
uint8_t tamanhoComando = 0;
bool comandoMuitoLongo = false;

// Ordem da matriz: pontos Braille {1, 2, 3, 4, 5, 6}.
// Mapeamento no PCA9685: ponto 1 = canal 0, ..., ponto 6 = canal 5.
const uint8_t braille_A[6] = {1, 0, 0, 0, 0, 0};
const uint8_t braille_B[6] = {1, 1, 0, 0, 0, 0};
const uint8_t braille_C[6] = {1, 0, 0, 1, 0, 0};
const uint8_t braille_D[6] = {1, 0, 0, 1, 1, 0};
const uint8_t braille_E[6] = {1, 0, 0, 0, 1, 0};
const uint8_t braille_F[6] = {1, 1, 0, 1, 0, 0};
const uint8_t braille_G[6] = {1, 1, 0, 1, 1, 0};
const uint8_t braille_H[6] = {1, 1, 0, 0, 1, 0};
const uint8_t braille_I[6] = {0, 1, 0, 1, 0, 0};
const uint8_t braille_J[6] = {0, 1, 0, 1, 1, 0};
const uint8_t braille_K[6] = {1, 0, 1, 0, 0, 0};
const uint8_t braille_L[6] = {1, 1, 1, 0, 0, 0};
const uint8_t braille_M[6] = {1, 0, 1, 1, 0, 0};
const uint8_t braille_N[6] = {1, 0, 1, 1, 1, 0};
const uint8_t braille_O[6] = {1, 0, 1, 0, 1, 0};
const uint8_t braille_P[6] = {1, 1, 1, 1, 0, 0};
const uint8_t braille_Q[6] = {1, 1, 1, 1, 1, 0};
const uint8_t braille_R[6] = {1, 1, 1, 0, 1, 0};
const uint8_t braille_S[6] = {0, 1, 1, 1, 0, 0};
const uint8_t braille_T[6] = {0, 1, 1, 1, 1, 0};
const uint8_t braille_U[6] = {1, 0, 1, 0, 0, 1};
const uint8_t braille_V[6] = {1, 1, 1, 0, 0, 1};
const uint8_t braille_W[6] = {0, 1, 0, 1, 1, 1};
const uint8_t braille_X[6] = {1, 0, 1, 1, 0, 1};
const uint8_t braille_Y[6] = {1, 0, 1, 1, 1, 1};
const uint8_t braille_Z[6] = {1, 0, 1, 0, 1, 1};

void moverServo(uint8_t canal, int angulo)
{
    if (!pcaConectado || canal >= QUANTIDADE_SERVOS)
    {
        return;
    }

    const uint16_t pwm = map(angulo, 0, 180, SERVOMIN, SERVOMAX);
    pca9685.setPWM(canal, 0, pwm);
}

void desligarTodosServos()
{
    if (!pcaConectado)
    {
        return;
    }

    for (uint8_t canal = 0; canal < QUANTIDADE_SERVOS; canal++)
    {
        // 4096 no registrador OFF desliga completamente o pulso do canal.
        pca9685.setPWM(canal, 0, 4096);
    }
}

const uint8_t *obterMatrizBraille(char letra)
{
    switch (letra)
    {
        case 'A': return braille_A;
        case 'B': return braille_B;
        case 'C': return braille_C;
        case 'D': return braille_D;
        case 'E': return braille_E;
        case 'F': return braille_F;
        case 'G': return braille_G;
        case 'H': return braille_H;
        case 'I': return braille_I;
        case 'J': return braille_J;
        case 'K': return braille_K;
        case 'L': return braille_L;
        case 'M': return braille_M;
        case 'N': return braille_N;
        case 'O': return braille_O;
        case 'P': return braille_P;
        case 'Q': return braille_Q;
        case 'R': return braille_R;
        case 'S': return braille_S;
        case 'T': return braille_T;
        case 'U': return braille_U;
        case 'V': return braille_V;
        case 'W': return braille_W;
        case 'X': return braille_X;
        case 'Y': return braille_Y;
        case 'Z': return braille_Z;
        default: return nullptr;
    }
}

void atuarMatrizBraille(char letra)
{
    if (!pcaConectado)
    {
        Serial.println("ERRO: PCA9685 nao foi encontrado. Servos nao acionados.");
        return;
    }

    const uint8_t *matriz = obterMatrizBraille(letra);
    if (matriz == nullptr)
    {
        Serial.println("ERRO: letra invalida.");
        return;
    }

    // Abaixa todos os servos da letra anterior
for (uint8_t canal = 0; canal < QUANTIDADE_SERVOS; canal++)
{
    moverServo(canal, ANGULO_REPOUSO);
}

delay(400);

    Serial.print("Letra recebida: ");
    Serial.println(letra);
    Serial.print("Pontos ativos:");

    // Posiciona todos os servos conforme a letra. Os pontos ativos sobem e os
    // inativos ficam em repouso.
    for (uint8_t canal = 0; canal < QUANTIDADE_SERVOS; canal++)
    {
        if (matriz[canal] == 1)
        {
            Serial.print(' ');
            Serial.print(canal + 1);
            moverServo(canal, ANGULO_PONTO_ATIVO);
        }
        else
        {
            moverServo(canal, ANGULO_REPOUSO);
        }
        delay(40);
    }
    Serial.println();

    Serial.println("Aguardando a próxima letra...");
}

void testarServo(uint8_t canal)
{
    if (!pcaConectado)
    {
        Serial.println("ERRO: PCA9685 nao foi encontrado. Servo nao acionado.");
        return;
    }

    Serial.print("Testando somente o servo do canal ");
    Serial.println(canal);

    moverServo(canal, ANGULO_REPOUSO);
    delay(700);
    moverServo(canal, ANGULO_PONTO_ATIVO);
    delay(1200);
    moverServo(canal, ANGULO_REPOUSO);
    delay(700);
    pca9685.setPWM(canal, 0, 4096);

    Serial.println("Teste individual concluido.");
}

void limparFilaSerial()
{
    while (Serial.available() > 0)
    {
        Serial.read();
    }
}

uint8_t diagnosticarBarramentoI2C()
{
    uint8_t encontrados = 0;

    Serial.println("Procurando dispositivos I2C...");
    for (uint8_t endereco = 1; endereco < 127; endereco++)
    {
        Wire.beginTransmission(endereco);
        const uint8_t resultado = Wire.endTransmission();

        if (resultado == 0)
        {
            Serial.print("  Encontrado em 0x");
            if (endereco < 0x10)
            {
                Serial.print('0');
            }
            Serial.println(endereco, HEX);
            encontrados++;
        }
    }

    if (encontrados == 0)
    {
        Serial.println("  Nenhum dispositivo I2C encontrado.");
    }

    return encontrados;
}

void relatorioEletricoI2C()
{
    Serial.println("=== Estado eletrico do I2C ===");
    Serial.print("SDA/GPIO");
    Serial.print(sdaEmUso);
    Serial.print(" = ");
    Serial.println(digitalRead(sdaEmUso) == HIGH ? "HIGH" : "LOW");
    Serial.print("SCL/GPIO");
    Serial.print(sclEmUso);
    Serial.print(" = ");
    Serial.println(digitalRead(sclEmUso) == HIGH ? "HIGH" : "LOW");

    Wire.beginTransmission(PCA9685_ENDERECO);
    const uint8_t resultado = Wire.endTransmission();
    Serial.print("Resposta de 0x40 = ");
    Serial.println(resultado);

    switch (resultado)
    {
        case 0:
            Serial.println("OK: o PCA9685 respondeu.");
            break;
        case 2:
            Serial.println("NACK: barramento ativo, mas nenhum dispositivo respondeu em 0x40.");
            break;
        case 4:
            Serial.println("ERRO de barramento: confira curto, GND e contato dos fios.");
            break;
        case 5:
            Serial.println("TIMEOUT: SDA ou SCL pode estar presa em nivel LOW.");
            break;
        default:
            Serial.println("Resposta I2C inesperada.");
            break;
    }
}

void processarComando()
{
    if (comandoMuitoLongo)
    {
        Serial.println("ERRO: comando muito longo. Use A-Z ou T0-T5.");
    }
    else if (tamanhoComando == 1 && comandoSerial[0] == '?')
    {
        relatorioEletricoI2C();
    }
    else if (tamanhoComando == 1 && isalpha(static_cast<unsigned char>(comandoSerial[0])))
    {
        const char letra = static_cast<char>(toupper(static_cast<unsigned char>(comandoSerial[0])));
        atuarMatrizBraille(letra);
    }
    else if (tamanhoComando == 2 &&
             toupper(static_cast<unsigned char>(comandoSerial[0])) == 'T' &&
             comandoSerial[1] >= '0' && comandoSerial[1] <= '5')
    {
        testarServo(static_cast<uint8_t>(comandoSerial[1] - '0'));
    }
    else if (tamanhoComando > 0)
    {
        Serial.println("ERRO: use A-Z, T0-T5 ou ? e pressione Enter.");
    }

    tamanhoComando = 0;
    comandoMuitoLongo = false;
    comandoSerial[0] = '\0';

    // Descarta caracteres digitados enquanto os servos estavam em movimento.
    limparFilaSerial();
    Serial.println("Pronto para o proximo comando.");
}

void setup()
{
    Serial.begin(115200);
    delay(1500);

    Serial.println();
    Serial.println("=== Diagnostico BrailleBox ===");
    Serial.print("I2C: SDA=GPIO");
    Serial.print(PINO_SDA);
    Serial.print(", SCL=GPIO");
    Serial.println(PINO_SCL);

    sdaEmUso = PINO_SDA;
    sclEmUso = PINO_SCL;
    Wire.begin(sdaEmUso, sclEmUso);
    diagnosticarBarramentoI2C();

    pcaConectado = pca9685.begin();
    if (!pcaConectado)
    {
        Serial.println("ERRO: PCA9685 nao respondeu no endereco 0x40.");
        Serial.println("Confira VCC, GND, SDA, SCL e os jumpers de endereco.");
    }
    else
    {
        pca9685.setPWMFreq(50);
        delay(10);
        desligarTodosServos();
        Serial.println("OK: PCA9685 encontrado no endereco 0x40.");
    }

    Serial.println("Modo: 6 servos nos canais 0-5 (pontos Braille 1-6).");
    Serial.println("Comandos: A-Z = letra; T0-T5 = teste individual; ? = diagnostico I2C.");
    Serial.println("Digite um comando e pressione Enter.");
}

void loop()
{
    while (Serial.available() > 0)
    {
        const char recebido = static_cast<char>(Serial.read());

        if (recebido == '\r' || recebido == '\n')
        {
            if (tamanhoComando > 0 || comandoMuitoLongo)
            {
                processarComando();
            }
            continue;
        }

        if (!isPrintable(recebido))
        {
            continue;
        }

        if (tamanhoComando < sizeof(comandoSerial) - 1)
        {
            comandoSerial[tamanhoComando++] = recebido;
            comandoSerial[tamanhoComando] = '\0';
        }
        else
        {
            comandoMuitoLongo = true;
        }
    }
}
