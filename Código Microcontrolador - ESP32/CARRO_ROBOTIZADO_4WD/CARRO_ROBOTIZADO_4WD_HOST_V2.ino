/**************************************************
     AUTHOR: ERIVELTO CLÉNIO DA COSTA E SILVA
     FOR: RODRIGO CONTROLE REMOTO DE CARRO ROBOTIZADO 4WD
     CREATED AT: 03-06-2023.
     Objectivo: Controlar um carro robotizado de 4 rodas por meio do uma página web
   no computador ou celular, isso via WIFI.


  Principais Componentes usados:
 *** 1-ESP32 (1)
 *** 2-DRIVER L293D (1)
 *** 3-KIT CARRO ROBOTIZADO 4WD (1)
 *** 4-ULTRASSONICO (1)
 *************************************************/

////////////////// Labraries Used  ////////////////
#include <WiFi.h>                             /////
#include <AsyncTCP.h>                         /////
#include <Ultrasonic.h>                       /////
#include <ESPAsyncWebServer.h>                /////
///////////////////////////////////////////////////

///////////////////Definitions/////////////////////
#define LED 2                                 /////
#define MOTOR_E_A 15                          /////
#define MOTOR_E_B 4                           /////
#define MOTOR_D_A 5                           /////
#define MOTOR_D_B 18                          /////
#define BUZZER 13                             /////
#define FAROL_TRAZEIRO_ESQUERDO 19            /////
#define FAROL_TRAZEIRO_DIREITO 21             /////
#define TRIGGER 22                            /////
#define ECHO 23                               /////
///////////////////////////////////////////////////


/////////////  NETWORK CONFIGURATIONS /////////////
const char* ssid = "CAR";                     /////
const char* password = "987654321";           /////
///////////////////////////////////////////////////

////////// NETWORK USENAME AND PASSWORD ///////////
const char* PAGE_USERNAME = "admin";          /////
const char* PAGE_PASSWORD = "admin";          /////
///////////////////////////////////////////////////

////////// VARIABLES USED IN THE PROJECT //////////
unsigned long int tempoDelay = 0;             /////
byte cont = 0;                                /////
char estadoCarro = 'D';                       /////
///////////////////////////////////////////////////


///////////////  OBJECTS DEFINITIONS  /////////////
Ultrasonic ultrasonico(TRIGGER, ECHO);        /////
AsyncWebServer server(80);                    /////
///////////////////////////////////////////////////

//////////////////////////////// FUNCTION DEFINITIONS  ////////////////////////////////////////////////
void tocarBuzina();
void desativarBuzina();
boolean hasObstaculo;
void carroFrente();
void carroEsquerda();
void carroDireita();
void carroParar();
void carroRecuar();                                                                              /////
void serverHandlers();                                                                           /////
void wifiConfigurations();                                                                        /////
void initialConfigurations();                                                                     /////
///////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-pt">

<head>
    <meta charset="UTF-8">
    <meta name="description" content="Página para controle de Carro Robotizado!">
    <meta name="keywords" content="Página, Carro,Robotico, Robotizado, 4WD, Controle, WIFI, Projecto">
    <meta name="author" content="Erivelto Clénio da Costa e Silva">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MyCar - Controle de Carro</title>

    <style>
        * {
            padding: 0;
            margin: 0;
            box-sizing: border-box;
        }
         body {
            background: linear-gradient(to bottom, #0f60ca, #123772);
        }

        header img {
            width: 100px;
            height: 100px;
        }

        header h2 {
            color: white;
            
            margin-top: 5px;
            text-align: center;
            
        }

        .centralizado {
            width: 80%;
            margin: auto;
        }

        .contentor {
            height: 460px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
        }

        .contentor-botao {
            width: 100%;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
        }

        .botao {
            background-color: #111b29;
            width: 50%;
            color: white;
            padding: 15px;
            border-radius: 5px;
            border-width: 0px;
        }

        .lado-a-lado {
            width: 100%;
            display: flex;
            flex-direction: row;
            justify-content: center;
            align-items: center;
        }

        .m-vertical {
            margin-top: 10px;
            margin-bottom: 10px;
        }

        .m-horizontal {
            margin-left: 10px;
            margin-right: 10px;
        }
    </style>
</head>

<body>
<header class="centralizado">
        <h2>Controle <strong> do Carro</strong></h2>
    </header>
    <main class="centralizado contentor">
        <div class="contentor-botao">
            <button id="botaoFrente" class="botao">
                Frente
            </button>
        </div>

        <div class="contentor-botao m-vertical">
            <div class="lado-a-lado">
                <button id="botaoEsquerda" class="botao  m-horizontal">
                    Esquerda
                </button>
                <button id="botaoDireita" class="botao  m-horizontal">
                    Direita
                </button>
            </div>
        </div>

        <div class="contentor-botao m-vertical">
            <button id="botaoRecuar" class="botao">
                Recuar
            </button>
        </div>

        <div class="contentor-botao m-vertical">
            <button id="botaoParar" type="button" class="botao">
                Parar
            </button>
        </div>

        <div class="row">
            <div class="col form-check">
                <label class="form-check-label" for="flexCheckDefault">

                </label>
            </div>
            <div class=" col form-check">
                <input class="form-check-input" type="checkbox" value="" id="checkPresenca" nome="estadoPresenca">
                <label class="form-check-label" for="estadoPresenca">
                    Sen.Obstaculo
                </label>
            </div>
        </div>
    </main>


    <script>
        function requisicaoGet(stringRequisicao) {
            var req = new XMLHttpRequest();
            req.onreadystatechange = function () {
                if (this.status == 200 && this.readyState == 4)
                    if (this.getResponseHeader(stringRequisicao) != null)
                        console.log(this.getResponseHeader(stringRequisicao));
            }
            req.open("GET", "/" + stringRequisicao, true);
            req.send(null);
        }

        window.onload = function () {
            document.getElementById("botaoFrente").addEventListener("click", () => { requisicaoGet("carroFrente"); });
            document.getElementById("botaoEsquerda").addEventListener("click", () => { requisicaoGet("carroEsquerda"); });
            document.getElementById("botaoDireita").addEventListener("click", () => { requisicaoGet("carroDireita"); });
            document.getElementById("botaoParar").addEventListener("click", () => { requisicaoGet("carroParar"); });
            document.getElementById("botaoRecuar").addEventListener("click", () => { requisicaoGet("carroRecuar"); });
            document.getElementById("checkPresenca").onchange = () => {
                let valorCheck = (document.getElementById("checkPresenca").checked) ? "1" : "0";
                requisicaoGet("estadoPresenca?estado=" + valorCheck);
            }
        }
    </script>
</body>

</html>
)rawliteral";


////////////////////////////////////////////////////////////////////////////////
void setup(){
  initialConfigurations();
  wifiConfigurations();
  serverHandlers();
  Serial.println("\nSISTEMA DE DOMÓTICA!");
}

///////////////////////////////////////////////////////////////////////////////  
void loop() {
  if(millis()-tempoDelay>500)
  {
      tempoDelay = millis();
      digitalWrite(LED, !digitalRead(LED));
      long distancia = ultrasonico.read();
      Serial.println("OSTACULO A:"+String(distancia)+"cm");
      if(distancia <20 && estadoCarro=='L')
      {
        carroParar();
        tocarBuzina(); 
      }
      else{
        desativarBuzina();  
      }
  }
  delay(20);
}

/////////////////////////////////////////////////////////
void initialConfigurations(){
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(FAROL_TRAZEIRO_ESQUERDO, OUTPUT);
  pinMode(FAROL_TRAZEIRO_DIREITO, OUTPUT);
  pinMode(MOTOR_E_A, OUTPUT);
  pinMode(MOTOR_E_B, OUTPUT);
  pinMode(MOTOR_D_A, OUTPUT);
  pinMode(MOTOR_D_B, OUTPUT);
  
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(FAROL_TRAZEIRO_ESQUERDO, LOW);
  digitalWrite(FAROL_TRAZEIRO_DIREITO, LOW);
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, LOW);
  Serial.begin(115200);delay(1000);
}

/////////////////////////////////////////////////////////
void wifiConfigurations()
{
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}


/////////////////////////////////////////////////////////
void serverHandlers()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(PAGE_USERNAME, PAGE_PASSWORD))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html);
  });
  
  server.on("/carroFrente", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("\n#####Carro Indo para Frente#####");
    digitalWrite(LED, !digitalRead(LED));
    carroFrente();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    response->addHeader("carroFrente","Carro Indo para Frente");
    request->send(response);
  });
  
  server.on("/carroEsquerda", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("\n#####Carro para Esquerda#####");
    digitalWrite(LED, !digitalRead(LED));
    carroEsquerda();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    response->addHeader("carroEsquerda","Carro Indo para Esquerda");
    request->send(response);
  });

  server.on("/carroParar", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("\n#####Carro para Parar#####");
    digitalWrite(LED, !digitalRead(LED));
    carroParar();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    response->addHeader("carroParar","Carro Parado!");
    request->send(response);
  });

  server.on("/carroDireita", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("\n#####Carro Indo para Direita#####");
    digitalWrite(LED, !digitalRead(LED));
    carroDireita();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    response->addHeader("carroDireita","Carro Indo para Direita");
    request->send(response);
  });

  server.on("/carroRecuar", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("\n#####Carro Recuar#####");
    digitalWrite(LED, !digitalRead(LED));
    carroRecuar();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    response->addHeader("carroRecuar","CARRO RECUANDO");
    request->send(response);
  });
  

  server.on("/estadoPresenca", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("\n#####Requisicao de programar recebida#####");
    digitalWrite(LED, !digitalRead(LED));
    ///estadoPresenca?estado=true
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    if(request->getParam("estado")->value().toInt())
    {
      estadoCarro = 'L';
      Serial.println(" ## SENSOR DE OBSTACULOS LIGADO! ");
    }else
    {
      estadoCarro = 'D';
      Serial.println(" ## SENSOR DE OBSTACULOS DESLIGADO! ");
    }
    response->addHeader("estadoPresenca", "ESTADO DO SENSOR DE PRESENÇA ALTERADO!");
    request->send(response);
  });

  // Start server
  server.begin();
}

////////////////////////////////////////////////////////////////////////
void carroFrente()
{
  digitalWrite(MOTOR_E_A, HIGH);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, HIGH);
  digitalWrite(MOTOR_D_B, LOW);
  digitalWrite(FAROL_TRAZEIRO_ESQUERDO, LOW);
  digitalWrite(FAROL_TRAZEIRO_DIREITO, LOW);
}

////////////////////////////////////////////////////////////////////////
void carroRecuar()
{
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, HIGH);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, HIGH);
  digitalWrite(FAROL_TRAZEIRO_ESQUERDO, HIGH);
  digitalWrite(FAROL_TRAZEIRO_DIREITO, HIGH);
}

////////////////////////////////////////////////////////////////////////
void carroEsquerda()
{
  digitalWrite(MOTOR_E_A, HIGH);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, LOW);
  digitalWrite(FAROL_TRAZEIRO_ESQUERDO, LOW);
  digitalWrite(FAROL_TRAZEIRO_DIREITO, HIGH);
}

////////////////////////////////////////////////////////////////////////
void carroDireita()
{
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, HIGH);
  digitalWrite(MOTOR_D_B, LOW);
  digitalWrite(FAROL_TRAZEIRO_ESQUERDO, HIGH);
  digitalWrite(FAROL_TRAZEIRO_DIREITO, LOW); 
}

////////////////////////////////////////////////////////////////////////
void carroParar()
{
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, LOW);
  digitalWrite(FAROL_TRAZEIRO_ESQUERDO, HIGH);
  digitalWrite(FAROL_TRAZEIRO_DIREITO, HIGH);
  tocarBuzina();
}

void tocarBuzina()
{
  digitalWrite(BUZZER, HIGH);
}

void desativarBuzina()
{
  digitalWrite(BUZZER, LOW);
}
