# 🔐 Cofre Morse — Arduino UNO

Projeto desenvolvido pela **LC Sistemas — @br.lcsistemas**.

Um cofre eletrônico interativo que utiliza Código Morse para desafios, treinamento e desbloqueio de uma fechadura por relé.

## 📦 Componentes

- 1 Arduino UNO
- 1 Display OLED I2C 0,96" (SSD1306, 128×64)
- 1 Buzzer passivo
- 1 Módulo relé 5V
- 2 Botões (vermelho e verde)
- 2 baterias lithium 26650 (4.2v cada)
- 1 Conector P4 macho
- 1 Interruptor de alavanca (liga/desliga)
- Jumpers e fios para conexões
- Fechadura elétrica

## 📚 Bibliotecas

Instale pelo Gerenciador de Bibliotecas do Arduino IDE:

- `Adafruit GFX Library`
- `Adafruit SSD1306`

Bibliotecas incluídas no ambiente Arduino:

- `Wire.h`
- `EEPROM.h`
- `avr/pgmspace.h`

## 🔌 Esquema elétrico

| Componente | Arduino UNO |
|---|---|
| OLED SDA | A4 |
| OLED SCL | A5 |
| OLED VCC | 5V* |
| OLED GND | GND |
| Botão verde | D6 e GND |
| Botão vermelho | D7 e GND |
| Buzzer (+) | D10 |
| Buzzer (-) | GND |
| Relé IN | D2 |
| Relé VCC | 5V |
| Relé GND | GND |

*Verifique se o OLED aceita alimentação de 5V.*

### 🔋 Alimentação

Os packs de pilhas alimentam o projeto por meio do conector P4 macho.

O interruptor de alavanca permite ligar e desligar o sistema.

**Atenção:** confirme a tensão, a polaridade e a capacidade real dos packs antes de conectá-los. Utilize regulador de tensão ou circuito de proteção quando necessário. Não conecte os packs diretamente em paralelo ou série sem verificar suas especificações.

A fechadura utiliza alimentação compatível, controlada pelos contatos do relé.

## 🎮 Modos de jogo

**1. TREINO**

O Arduino sorteia palavras simples de 3 a 4 letras. O jogador visualiza a palavra e precisa digitá-la em Código Morse.

**2. ALFABETO**

Treinamento das letras de A até Z. O display mostra a letra e seu código Morse. O jogador só avança após acertar.

**3. VALENDO**

O Arduino sorteia uma palavra e transmite pelo buzzer, sem mostrá-la.

- Começa com 10 pontos.
- Cada acerto reduz 1 ponto.
- A pontuação fica salva na EEPROM.
- Ao chegar a zero, o cofre é desbloqueado.

**4. UNLOCK**

Permite abrir diretamente o cofre digitando o segredo:

`----.` (4 traços e 1 ponto)

Se errar, retorna ao menu.

## 🔴🟢 Controles

| Botão | Menu | Digitação |
|---|---|---|
| 🟢 Verde | Próxima opção | Traço `-` |
| 🔴 Vermelho | Selecionar | Ponto `.` |

Uma pausa de aproximadamente 1,15 segundo confirma cada letra.

## 🔓 Abertura do cofre

Ao desbloquear:

1. O buzzer toca uma melodia por 4 segundos.
2. O relé é acionado por 4 segundos.
3. O relé desliga e o sistema retorna ao menu.

No modo VALENDO, a pontuação volta para 10 após a abertura.

<img width="4284" height="5712" alt="IMG_3796" src="https://github.com/user-attachments/assets/57c031f4-1dd2-4b47-ba6f-96fe15b192c7" />

<img width="3024" height="4032" alt="IMG_3795" src="https://github.com/user-attachments/assets/85373ef8-0792-4552-84f8-4f1e38fb04fb" />

<img width="3024" height="4032" alt="IMG_3797" src="https://github.com/user-attachments/assets/f859fff3-90a6-469e-a904-c9df4c4d1138" />

<img width="4284" height="5712" alt="IMG_3793" src="https://github.com/user-attachments/assets/2522d5bc-3cdf-492a-8a93-b64f3c46125f" />



---

**Desenvolvido por LC Sistemas**

Instagram: **@br.lcsistemas**
