/**************************************************
   * AUTHOR: ERIVELTO CLÉNIO DA COSTA E SILVA
   * FOR: RODRIGO CONTROLE REMOTO DE CARRO ROBOTIZADO 4WD
   * CREATED AT: 03-06-2023
   * Objectivo: Controlar um carro robotizado de 4 rodas por meio do uma página web
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
#include <ESPAsyncWebServer.h>                /////
///////////////////////////////////////////////////                                                                                                                                                                                                                                         

///////////////////Definitions/////////////////////
#define LED 2                            //////////
#define MOTOR_E_A 15 
#define MOTOR_E_B 4
#define MOTOR_D_A 5
#define MOTOR_D_B 18
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
unsigned long int tempoDelay=0;               /////
byte cont=0;                                  /////
char estadoCarro = 'A';                       /////
///////////////////////////////////////////////////


///////////////  OBJECTS DEFINITIONS  /////////////
AsyncWebServer server(80);                    /////
///////////////////////////////////////////////////

//////////////////////////////// FUNCTION DEFINITIONS  ////////////////////////////////////////////////
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
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Carro - Controle de Carro</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet"
        integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM"
        crossorigin="anonymous"></script>
    <link rel="stylesheet" href="./bootstrap.min.css">
    <style>
        * {
            padding: 0;
            margin: 0;
            box-sizing: border-box;
        }

        header,
        main {
            width: 80%;
            margin: auto;
        }

        header,
        .tempo,
        .horario {
            /* background-color: rgb(245, 247, 247); */
            /* background: linear-gradient(to bottom, #4e83d8, #061120); */
        }

        main {
            background: linear-gradient(to bottom, #0f60ca, #123772);
        }

        header img {
            width: 100px;
            height: 100px;
        }

        header h3 {
            margin-top: 50px;
            text-align: center;
        }

        .data h5,
        .hora h5 {
            background: linear-gradient(to bottom, #4e83d8, #061120);
        }

        .data div,
        .hora div,
        .botao {
            /* background: linear-gradient(to bottom, #838181, #c4d1e4, #838181); */
            background: linear-gradient(to bottom, #4e83d8, #061120);
        }

        .data p,
        .hora p {
            font-weight: bold;
            font-size: x-large;
        }

        .horario {
            color: white
        }
    </style>
</head>

<body class="container-fluid w-75 mt-5 mx-auto">

    <header class="d-flex">
        <img class="mx-5" src="" alt="">
        <h3 class="mx-auto mb-0">Controle <strong> do Carro</strong></h3>
    </header>

    <main class="text-light">
        <div class="d-grid gap-2 col-6 mx-auto mt-3">
            <h4 class="text-center">Controles</h4>
            <button id="botaoFrente" class="btn btn-dark mt-2 ">
                Frente
            </button>
        </div>


        <div class="horario mt-3">
            <div class="d-flex pb-2 botoes w-100">
                <button id="botaoEsquerda" class="w-50 btn btn-dark mt-2">
                    Esquerda
                </button>
                <button id="botaoDireita" class="w-50 btn btn-dark  ms-2 mt-2">
                    Direita
                </button>
            </div>
        </div>
        <div class="d-grid gap-2 col-6 mx-auto mt-3">
            <button id="botaoRecuar" class="btn btn-dark mt-2 ">
                Recuar
            </button>
            <button id="botaoParar" type="button" class="btn btn-dark mt-2">
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
                    S.Prensença
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
                        alert(this.getResponseHeader(stringRequisicao));
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
            document.getElementById("checkPresenca").onchange = ()=>{
                let valorCheck = (document.getElementById("checkPresenca").checked) ? "1" : "0";
                requisicaoGet("estadoPresenca?estado="+valorCheck);
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

////////////////////////////////////////////////////////////////////////////////  
void loop() {
  if(millis()-tempoDelay>1500)
  {
      tempoDelay = millis();
      digitalWrite(LED, !digitalRead(LED));
  }
  delay(20);
}

/////////////////////////////////////////////////////////
void initialConfigurations(){
  pinMode(LED, OUTPUT);
  pinMode(MOTOR_E_A, OUTPUT);
  pinMode(MOTOR_E_B, OUTPUT);
  pinMode(MOTOR_D_A, OUTPUT);
  pinMode(MOTOR_D_B, OUTPUT);
  
  digitalWrite(LED, LOW);
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
    response->addHeader("carroEsquerda","Carro Indo para Direita");
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
      estadoCarro = 'A';
    else
      estadoCarro = 'M';
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
}

////////////////////////////////////////////////////////////////////////
void carroRecuar()
{
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, HIGH);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, HIGH);
}

////////////////////////////////////////////////////////////////////////
void carroEsquerda()
{
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, HIGH);
  digitalWrite(MOTOR_D_B, LOW);
}

////////////////////////////////////////////////////////////////////////
void carroDireita()
{
  digitalWrite(MOTOR_E_A, HIGH);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, LOW);
}

////////////////////////////////////////////////////////////////////////
void carroParar()
{
  digitalWrite(MOTOR_E_A, LOW);
  digitalWrite(MOTOR_E_B, LOW);
  digitalWrite(MOTOR_D_A, LOW);
  digitalWrite(MOTOR_D_B, LOW);
}
