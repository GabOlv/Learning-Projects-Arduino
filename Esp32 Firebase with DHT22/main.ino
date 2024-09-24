#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <time.h>

// Definições de pinos e constantes
#define LED_BRANCO 33
#define LED_VERDE 5
#define LED_AMARELO 19
#define LED_VERMELHO 23
#define BUZZER 2
#define DHTPIN 15
#define DHTTYPE DHT22

// Inicialização de componentes
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);

// Configurações do Wi-Fi
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* ap_ssid = "ESP32_Hotspot";
const char* ap_password = "123456789";

// Configurações do Firebase
#define API_KEY "API_KEY"
#define DATABASE_URL "DATABASE_URL"
String databasePath = "/Temperatura"; // Ajuste conforme necessário

// Variáveis de controle
unsigned long elapsedMillis = 0;
const unsigned long update_interval = 10000; // Enviar dados a cada 10 segundos
unsigned long lastLedUpdate = 0;
const unsigned long ledUpdateInterval = 500;  // Intervalo para LEDs e LCD
unsigned long lastDataSend = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BRANCO, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    dht.begin();
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Iniciando...");

    // Configurar modo AP + STA
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("Hotspot iniciado. IP: ");
    Serial.println(WiFi.softAPIP());

    // Conectar à rede Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao Wi-Fi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("not connected");
    }
    
    Serial.println("\nConectado à rede Wi-Fi!");
    Serial.print("IP da rede: ");
    Serial.println(WiFi.localIP());

    // Exibir o IP do Hotspot no LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP: ");
    lcd.print(WiFi.softAPIP());

    // Configurar servidor Web
    server.on("/api/temperatura", HTTP_GET, handleGetTemperature);
    server.on("/api/temperaturaBD", HTTP_GET, handleGetTemperatureBD);
    server.on("/", handleRoot); // Página principal
    server.begin();

    // Inicializar a hora
    configTime(0, 0, "pool.ntp.org");
}

void loop() {
    server.handleClient();
    unsigned long currentMillis = millis();

    // Atualizar a temperatura e os LEDs a cada 500 ms
    if (currentMillis - lastLedUpdate >= ledUpdateInterval) {
        lastLedUpdate = currentMillis;

        float temperatura = dht.readTemperature();
        if (!isnan(temperatura)) {
            lcd.setCursor(0, 0);
            lcd.print("IP: ");
            lcd.print(WiFi.softAPIP());
            lcd.setCursor(0, 1);
            lcd.print("Temp: ");
            lcd.print(temperatura);
            lcd.print(" C   ");

            // Controle dos LEDs individualmente
            if (temperatura <= 34) {
                digitalWrite(LED_VERDE, HIGH);
                digitalWrite(LED_AMARELO, LOW);
                digitalWrite(LED_VERMELHO, LOW);
                digitalWrite(BUZZER, LOW);
            } else if (temperatura > 34 && temperatura < 36) {
                digitalWrite(LED_VERDE, LOW);
                digitalWrite(LED_AMARELO, HIGH);
                digitalWrite(LED_VERMELHO, LOW);
                digitalWrite(BUZZER, LOW);
            } else {
                digitalWrite(LED_VERDE, LOW);
                digitalWrite(LED_AMARELO, LOW);
                digitalWrite(LED_VERMELHO, HIGH);
                digitalWrite(BUZZER, HIGH);
            }
        }
    }

    // Envio de dados para o Firebase a cada 10 segundos
    if (currentMillis - lastDataSend >= update_interval) {
        lastDataSend = currentMillis;
        database_test(); // Função para enviar os dados

        // Piscar o LED branco ao enviar dados
        for (int i = 0; i < 5; i++) {
            digitalWrite(LED_BRANCO, HIGH);
            delay(100);
            digitalWrite(LED_BRANCO, LOW);
            delay(100);
        }
    }
}

String getFormattedDateTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Falha ao obter a hora");
        return "";
    }
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y - %H:%M:%S", &timeinfo);
    return String(buffer);
}

void handleGetTemperature() {
    float temperatura = dht.readTemperature();
    if (isnan(temperatura)) {
        server.send(500, "application/json", "{\"error\":\"Falha ao ler o sensor DHT22\"}");
    } else {
        String jsonResponse = "{\"temperatura\":" + String(temperatura) + "}";
        server.send(200, "application/json", jsonResponse);
    }
}

void handleGetTemperatureBD() {
    HTTPClient http;
    String url = DATABASE_URL + databasePath + ".json?auth=" + API_KEY;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            server.send(200, "application/json", payload);
        } else {
            server.send(500, "application/json", "{\"error\":\"Erro ao obter dados\"}");
        }
    } else {
        server.send(500, "application/json", "{\"error\":\"" + String(http.errorToString(httpCode).c_str()) + "\"}");
    }

    http.end();
}

void database_test() {
    float temperatura = dht.readTemperature();
    bool alerta_temperatura = false; // Flag para alerta de temperatura
    String dateTime = getFormattedDateTime();

    if (!isnan(temperatura)) {
        // Verifica se a temperatura está acima do limite
        if (temperatura >= 36) {
            alerta_temperatura = true; // Aciona a flag se a temperatura for alta
        }

        // Enviar temperatura, data/hora e alerta
        String json = "{\"temperatura\":" + String(temperatura) + 
                       ", \"data_hora\":\"" + dateTime + 
                       "\", \"alerta_temperatura\":" + String(alerta_temperatura) + "}";

        HTTPClient http;
        String url = DATABASE_URL + databasePath + ".json?auth=" + API_KEY;
        http.begin(url);
        
        int httpCode = http.POST(json);

        if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
            Serial.println("Dados enviados com sucesso!");
            Serial.print("Temperatura: ");
            Serial.print(temperatura);
            Serial.print(", Data/Hora: ");
            Serial.println(dateTime);
            Serial.print(", Alerta: ");
            Serial.println(alerta_temperatura ? "VERDADEIRO" : "FALSO");
        } else {
            Serial.println("Falha ao enviar dados: " + String(http.errorToString(httpCode).c_str()));
        }

        http.end();
    }
}

void handleRoot() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="pt-br">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Controle de Temperatura</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                background-color: #2b2b2b;
                color: #ffffff;
                margin: 0;
                padding: 20px;
                display: flex;
                flex-direction: column;
                align-items: center;
            }
            .container {
                background-color: #3b3b3b;
                padding: 20px;
                border-radius: 10px;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
                max-width: 500px;
                width: 100%;
            }
            h1 {
                text-align: center;
                color: #ffffff;
            }
            canvas {
                width: 100%;
                height: 200px;
                border: 1px solid #ccc;
                background-color: #444;
            }
            .status {
                margin-top: 20px;
                text-align: center;
                font-size: 18px;
            }
            .status span {
                font-weight: bold;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Monitoramento de Temperatura</h1>
            <canvas id="temperatureChart"></canvas>
            <div class="status">
                Temperatura Atual: <span id="currentTemperature">--</span> °C
            </div>
        </div>

        <script>
            let temperatures = Array(10).fill(0); // Alterado para 10 pontos
            const maxPoints = 10;

            function fetchTemperature() {
                fetch('/api/temperatura')
                    .then(response => response.json())
                    .then(data => {
                        const temp = data.temperatura;
                        document.getElementById('currentTemperature').textContent = temp;

                        // Adicionar nova temperatura e manter apenas maxPoints
                        temperatures.push(temp);
                        if (temperatures.length > maxPoints) temperatures.shift();

                        drawChart();
                    })
                    .catch(error => {
                        console.error('Erro ao buscar temperatura:', error);
                    });
            }

            function drawChart() {
                const canvas = document.getElementById('temperatureChart');
                const ctx = canvas.getContext('2d');
                ctx.clearRect(0, 0, canvas.width, canvas.height);

                const spacing = canvas.width / (maxPoints - 1); // Espaçamento maior entre os pontos
                temperatures.forEach((temp, index) => {
                    const x = index * spacing; // Aumentar o espaçamento
                    const y = canvas.height - ((temp - 20) * (canvas.height / 25)); // Proporção de 20 a 45
                    ctx.fillStyle = temp >= 36 ? 'red' : (temp >= 34 ? 'yellow' : 'green');
                    ctx.beginPath();
                    ctx.arc(x, y, 5, 0, 2 * Math.PI);
                    ctx.fill();
                    ctx.fillStyle = 'white';
                    ctx.fillText(temp.toFixed(1) + '°C', x - 10, y - 10);
                });

                // Desenhar linhas
                ctx.strokeStyle = 'white';
                ctx.lineWidth = 2;
                ctx.beginPath();
                temperatures.forEach((temp, index) => {
                    const x = index * spacing; // Manter o mesmo espaçamento
                    const y = canvas.height - ((temp - 20) * (canvas.height / 25));
                    if (index === 0) {
                        ctx.moveTo(x, y);
                    } else {
                        ctx.lineTo(x, y);
                    }
                });
                ctx.stroke();
            }

            setInterval(fetchTemperature, 5000);
        </script>
    </body>
    </html>
    )rawliteral";
  
    server.send(200, "text/html", html);
}
