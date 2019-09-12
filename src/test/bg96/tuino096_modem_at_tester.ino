

#include <ArduinoSTL.h>
#include <vector>


#define BG96_RESET  11
#define BG96_PWR    12

const char* baltimoreCyberTrustRootCA = "-----BEGIN CERTIFICATE-----MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoXDTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9yZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVyVHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKrmD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjrIZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeKmpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSuXmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZydc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/yejl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT929hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3WgxjkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhzksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLSR9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp-----END CERTIFICATE-----\n";
const char* teleSecGlobalRootCA = "-----BEGIN CERTIFICATE-----MIIDwzCCAqugAwIBAgIBATANBgkqhkiG9w0BAQsFADCBgjELMAkGA1UEBhMCREUxKzApBgNVBAoMIlQtU3lzdGVtcyBFbnRlcnByaXNlIFNlcnZpY2VzIEdtYkgxHzAdBgNVBAsMFlQtU3lzdGVtcyBUcnVzdCBDZW50ZXIxJTAjBgNVBAMMHFQtVGVsZVNlYyBHbG9iYWxSb290IENsYXNzIDIwHhcNMDgxMDAxMTA0MDE0WhcNMzMxMDAxMjM1OTU5WjCBgjELMAkGA1UEBhMCREUxKzApBgNVBAoMIlQtU3lzdGVtcyBFbnRlcnByaXNlIFNlcnZpY2VzIEdtYkgxHzAdBgNVBAsMFlQtU3lzdGVtcyBUcnVzdCBDZW50ZXIxJTAjBgNVBAMMHFQtVGVsZVNlYyBHbG9iYWxSb290IENsYXNzIDIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCqX9obX+hzkeXaXPSi5kfl82hVYAUdAqSzm1nzHoqvNK38DcLZSBnuaY/JIPwhqgcZ7bBcrGXHX+0CfHt8LRvWurmAwhiCFoT6ZrAIxlQjgeTNuUk/9k9uN0goOA/FvudocP05l03Sx5iRUKrERLMjfTlH6VJi1hKTXrcxlkIF+3anHqP1wvzpesVsqXFP6st4vGCvx9702cu+fjOlbpSD8DT6IavqjnKgP6TeMFvvhk1qlVtDRKgQFRzlAVfFmPHmBiiRqiDFt1MmUUOyCxGVWOHAD3bZwI18gfNycJ5v/hqO2V81xrJvNHy+SE/iWjnX2J14np+GPgNeGYtEotXHAgMBAAGjQjBAMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgEGMB0GA1UdDgQWBBS/WSA2AHmgoCJrjNXyYdK4LMuCSjANBgkqhkiG9w0BAQsFAAOCAQEAMQOiYQsfdOhyNsZt+U2e+iKo4YFWz827n+qrkRk4r6p8FU3ztqONpfSO9kSpp+ghla0+AGIWiPACuvxhI+YzmzB6azZie60EI4RYZeLbK4rnJVM3YlNfvNoBYimipidx5joifsFvHZVwIEoHNN/q/xWA5brXethbdXwFeilHfkCoMRN3zUA7tFFHei4R40cR3p1m0IvVVGb6g1XqfMIpiRvpb7PO4gWEyS8+eIVibslfwXhjdFjASBgMmTnrpMwatXlajRWc2BQN9noHV8cigwUtPJslJj0Ys6lDfMjIq2SPDqO/nBudMNva0Bkuqjzx+zOAduTNrRlPBSeOE6Fuwg==-----END CERTIFICATE-----\n";
const char* teleSecServerPassCA2 = "-----BEGIN CERTIFICATE-----MIIFYDCCBEigAwIBAgIEByeyFjANBgkqhkiG9w0BAQsFADBaMQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTE0MDcwOTE3MTQ1MFoXDTIxMDcwOTE3MDkwNFowgdgxCzAJBgNVBAYTAkRFMSUwIwYDVQQKExxULVN5c3RlbXMgSW50ZXJuYXRpb25hbCBHbWJIMR4wHAYDVQQLExVUcnVzdCBDZW50ZXIgU2VydmljZXMxIDAeBgNVBAMTF1RlbGVTZWMgU2VydmVyUGFzcyBDQSAyMRwwGgYDVQQIExNOb3JkcmhlaW4gV2VzdGZhbGVuMQ4wDAYDVQQREwU1NzI1MDEQMA4GA1UEBxMHTmV0cGhlbjEgMB4GA1UECRMXVW50ZXJlIEluZHVzdHJpZXN0ci4gMjAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDOiwnq/1yGnxwkfemFGfSS2hk4YW3c42GhrWjVwzDb7Mp5h0TrM0Dl1CFk0tq7IllMhdvQUjSTjpmu6LswGeEI4riwnSsFXqDv83nzTEBx3hCR1N03ioT1Ob9iWCmEhCIskt2AsxPFXB2s1Dspiwg5ZY6OyC4AP08ySY+Sl+Fn0b2VUSRJJEaH2lY9NMAN9RKsC0mfTsYN6HAnXgCGVpYLc94LtbdkBO6sevsMn9TBRldq5IFzKd1Dx/r9OJszya1kZSvp/vrSP629JdANRWQVnBoLLJu3zp8k6dLG96AD3Z/lYPWurNeAjhP6OPoTg+mB6Lhu+hw06hrXGJzwPGpBAgMBAAGjggGtMIIBqTASBgNVHRMBAf8ECDAGAQH/AgEAMIGbBgNVHSAEgZMwgZAwSAYJKwYBBAGxPgEAMDswOQYIKwYBBQUHAgEWLWh0dHA6Ly9jeWJlcnRydXN0Lm9tbmlyb290LmNvbS9yZXBvc2l0b3J5LmNmbTBEBgkrBgEEAb1HDQIwNzA1BggrBgEFBQcCARYpaHR0cDovL3d3dy50ZWxlc2VjLmRlL3NlcnZlcnBhc3MvY3BzLmh0bWwwQgYIKwYBBQUHAQEENjA0MDIGCCsGAQUFBzABhiZodHRwOi8vb2NzcC5vbW5pcm9vdC5jb20vYmFsdGltb3Jlcm9vdDAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMB8GA1UdIwQYMBaAFOWdWTCCR1jMrPoIVDaGezq1BE3wMEIGA1UdHwQ7MDkwN6A1oDOGMWh0dHA6Ly9jZHAxLnB1YmxpYy10cnVzdC5jb20vQ1JML09tbmlyb290MjAyNS5jcmwwHQYDVR0OBBYEFBW73tYlb73yMZ9iE9xcpvRltG3yMA0GCSqGSIb3DQEBCwUAA4IBAQBNSC2Jb3V64H4ivhM2fK53fnvypHgiodSitLvY48vy6gWIlbZaac19iroZSDrQC/pcijupH92HQ2V8TPPIa/M1e4iV/ANQTRIyrBiQ76N0xpx7wQFn8EdRO0NjCJ3BS+MITK0W4+Dy8/xqTjGbcvTXe9O3I/o4CGUOI93LWbxEwulUg/XBm/usvqIu7KEWvl9KJ9E1MX+/xllkkwr3sEBtKzUsMriuQZIPagk4Sopmwkt94g8k0xC4L7zkmzOWbc3usSrEJSJmBcclCl0W3+D6enOnNQDQSeTtN82BTlYoBJHkStMogP3lYYCI8YZsHAyYxjwAiuSO3dzi1wBs0aiO-----END CERTIFICATE-----\n";
const char* wwwgooglecom = "-----BEGIN CERTIFICATE-----MIIDxzCCAq+gAwIBAgIIIivMz8oUXD4wDQYJKoZIhvcNAQELBQAwVDELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFUdvb2dsZSBUcnVzdCBTZXJ2aWNlczElMCMGA1UEAxMcR29vZ2xlIEludGVybmV0IEF1dGhvcml0eSBHMzAeFw0xODA2MTkxMTM4NDlaFw0xODA4MjgxMTMxMDBaMGgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApDYWxpZm9ybmlhMRYwFAYDVQQHDA1Nb3VudGFpbiBWaWV3MRMwEQYDVQQKDApHb29nbGUgTExDMRcwFQYDVQQDDA53d3cuZ29vZ2xlLmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABH8YFet+ncg1dlNCBaswIMgIv/uUaCvxXuPx5JtMKQ+Me9El1hOfDK+0JLeGCkD/glWIm7X2Qa7YuT9igWWywpOjggFSMIIBTjATBgNVHSUEDDAKBggrBgEFBQcDATAOBgNVHQ8BAf8EBAMCB4AwGQYDVR0RBBIwEIIOd3d3Lmdvb2dsZS5jb20waAYIKwYBBQUHAQEEXDBaMC0GCCsGAQUFBzAChiFodHRwOi8vcGtpLmdvb2cvZ3NyMi9HVFNHSUFHMy5jcnQwKQYIKwYBBQUHMAGGHWh0dHA6Ly9vY3NwLnBraS5nb29nL0dUU0dJQUczMB0GA1UdDgQWBBTz8xCmrOCD1lSOeVrlEzaDeOxShTAMBgNVHRMBAf8EAjAAMB8GA1UdIwQYMBaAFHfCuFCaZ3Z2sS3ChtCDoH6mfrpLMCEGA1UdIAQaMBgwDAYKKwYBBAHWeQIFAzAIBgZngQwBAgIwMQYDVR0fBCowKDAmoCSgIoYgaHR0cDovL2NybC5wa2kuZ29vZy9HVFNHSUFHMy5jcmwwDQYJKoZIhvcNAQELBQADggEBAJdN1p4Stlt5SUZyVwoiKE0seZc2ssc9qlVvc1/piWnDxUohR+AVdUGHG9ycpq2xzwsp+t6j4K2y8nn7enRkqN4nj/L1/jACjBdsmVb7dQ8aYr5WAhoMZRsnNlr83HhTGjT08estiQGCeYA0KjIz6AiERYz/gc2ihqRFh4MqWIx3xToD/OsJN/9+11Vqpx7wgc/I6smjyrNYSM5p5nbPIwHv5QQEWdjYJPogQqPYmxZa11PbCPVLh9k+DQyQpDrYzLEwLVCRZtzdwLW1FvbfjkoCz8Own1WOaCtCk3pmUsefzgCeOKbBJ6yaC3cVuaYZGgHLo7+UQxRAixI4RFbxW7w=-----END CERTIFICATE-----\n";
const char* sigRoCAR2 = "-----BEGIN CERTIFICATE-----MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4GA1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNpZ24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPLv4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklqtTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzdC9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pazq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCBmTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IHV2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5nbG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4GsJ0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavSot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxdAfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==-----END CERTIFICATE-----\n";
const char* goAutG3 = "-----BEGIN CERTIFICATE-----MIIEXDCCA0SgAwIBAgINAeOpMBz8cgY4P5pTHTANBgkqhkiG9w0BAQsFADBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEyMTUwMDAwNDJaMFQxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3QgU2VydmljZXMxJTAjBgNVBAMTHEdvb2dsZSBJbnRlcm5ldCBBdXRob3JpdHkgRzMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDKUkvqHv/OJGuo2nIYaNVWXQ5IWi01CXZaz6TIHLGp/lOJ+600/4hbn7vn6AAB3DVzdQOts7G5pH0rJnnOFUAK71G4nzKMfHCGUksW/mona+Y2emJQ2N+aicwJKetPKRSIgAuPOB6Aahh8Hb2XO3h9RUk2T0HNouB2VzxoMXlkyW7XUR5mw6JkLHnA52XDVoRTWkNty5oCINLvGmnRsJ1zouAqYGVQMc/7sy+/EYhALrVJEA8KbtyX+r8snwU5C1hUrwaW6MWOARa8qBpNQcWTkaIeoYvy/sGIJEmjR0vFEwHdp1cSaWIr6/4g72n7OqXwfinu7ZYW97EfoOSQJeAzAgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHfCuFCaZ3Z2sS3ChtCDoH6mfrpLMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd/cGYYuMDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdvb2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dzcjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEAHLeJluRT7bvs26gyAZ8so81trUISd7O45skDUmAge1cnxhG1P2cNmSxbWsoiCt2eux9LSD+PAj2LIYRFHW31/6xoic1k4tbWXkDCjir37xTTNqRAMPUyFRWSdvt+nlPqwnb8Oa2I/maSJukcxDjNSfpDh/Bd1lZNgdd/8cLdsE3+wypufJ9uXO1iQpnh9zbuFIwsIONGl1p3A8CgxkqI/UAih3JaGOqcpcdaCIzkBaR9uYQ1X4k2Vg5APRLouzVy7a8IVk6wuy6pm+T7HT4LY8ibS5FEZlfAFLSW8NwsVz9SBK2Vqn1N0PIMn5xA6NZVc7o835DLAFshEWfC7TIe3g==-----END CERTIFICATE-----\n";

const char* restCmd = "POST /s HTTP/1.1\r\n"
		"X-Id: CoT-MQTT-Arduino_Lib\r\n"
		"Content-Type: text/plain\r\n"
		"Authorization: Basic aGVpbmVyLmhlbmtlbnNpZWZrZW5AdC1zeXN0ZW1zLmNvbTpIZW5rQ2xvdWQ5OA==\r\n"
		"\r\n"
		"200,169331,SUCCESSFUL\r\n";


//initializing
void setup() {
    delay(100);
    //start logger uart
    Serial.begin(115200);
    delay(100);
    Serial.println("setup()...");
    //start modem uart
    Serial2.begin(115200);
    delay(100);
    modemReset(true);
    //modem rest pin
    pinMode(BG96_RESET, OUTPUT);
    modemReset(false);
    modemPower(false);
    //modem power pin
    pinMode(BG96_PWR, OUTPUT);
    modemPower(true);
    //LED_BUILTIN pin
    pinMode(LED_BUILTIN, OUTPUT);
    //Led blink
    int n = 10;
    while(n--) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN, LOW);
        delay(300);
    }
    //build md list
    initCmds();
    showHelp();
    Serial.println("setup()... done.");
}

//never ending loop
void loop() {
    sendAndReceive();
    readInput();
}

void modemPower(bool on) {
    digitalWrite(BG96_PWR, on);
}

void modemReset(bool reset) {
    digitalWrite(BG96_RESET, reset);
}

std::vector<String> cmds;
std::vector<String> cmdsToExecute;
std::vector<String> cmdsLastToExecute;
std::vector<String> cmdsRecorded;
unsigned long cmdStartTime = 0;

bool infoMode = true;
void sendAndReceive() {
    if (cmdsToExecute.size()) {
        unsigned long cmdsStartTime = millis();
        for (unsigned int i = 0; i < cmdsToExecute.size(); i++) {
            String cmd = cmdsToExecute[i];
            String toSend = cmd;
            //default wait for response
            unsigned long waitForResponse = 100;
            int cmdExecuteTimes = 1;
            if (cmd.startsWith("#")) {
                toSend = cmds[atoi(cmd.substring(1).c_str())];
            }
            int di = toSend.indexOf("#", 1);
            if (di > -1) {
                String parameter = toSend.substring(di + 1);
                int ti = parameter.indexOf("x", 1);
                if (ti > -1) {
                    cmdExecuteTimes = atoi(parameter.substring(ti + 1).c_str());
                    parameter.remove(ti);
                }
                waitForResponse = atol(parameter.c_str());
                toSend.remove(di);
            }
            unsigned int l = toSend.length();
            if (l) {
                int callNumber = 0;
                while(cmdExecuteTimes--) {
                    callNumber++;
                    String resonseBuffer = "";
                    if(infoMode) {
                        Serial.print("--- "); Serial.print(callNumber); Serial.print(".send cmd[");
                        Serial.print(cmd);
                        Serial.print("] "); Serial.print("wait for response<"); Serial.print(waitForResponse); Serial.print("> ----------------------------------");
                    }
                    Serial.println();
                    //sending...
                    cmdStartTime = millis();
                    for (unsigned int i = 0; i < l; i++) {
                        Serial2.write(toSend[i]);
                    }
                    Serial2.write('\r');
                    //receiving...
                    unsigned long cmdStartRecTime = millis();
                    unsigned long end = cmdStartRecTime + waitForResponse;
                    while(millis() < end) {
                        while (Serial2.available()>0) {
                            resonseBuffer.concat(Serial2.readStringUntil('\r'));
                        }
                    }
                    if(infoMode) {
                        Serial.print("--- ");Serial.print(callNumber);Serial.println(". resp ----------------------------------");
                    }
                    Serial.write(resonseBuffer.c_str());
                    if(infoMode) {
                        Serial.print("--- ");Serial.print(callNumber);Serial.print(". done send in<"); Serial.print(cmdStartRecTime - cmdStartTime);
                        Serial.print(">ms. rec in<"); Serial.print(millis() - cmdStartRecTime); Serial.print(">ms. ----------------------------------");
                    }
                    Serial.println();
                }
            } else {
                Serial.print("--- EMPTY cmd["); Serial.print(cmd); Serial.println("]----------------------------------");
            }
        }
        if (cmdsToExecute.size() > 1) {
            Serial.print("--- done all in <"); Serial.print(millis() - cmdsStartTime); Serial.println(">ms. ----------------------------------");
        }
        cmdsToExecute.clear();
    } else {
        if (Serial2.available()>0) {
            Serial.print("+++ last send <"); Serial.print(millis() - cmdStartTime); Serial.println(">ms. ago +++++++");
            while (Serial2.available()>0) {
               Serial.write(Serial2.read());
            }
            Serial.println("+++++++++++++++++++++++++++");
        }
    }
}

void showHelp() {
    Serial.println("---- help ----------------------------------");
    Serial.println(" '?' -> show help, cmd list and recorded cmd list");
    Serial.println(" '%p1' -> switch modem power pin on");
    Serial.println(" '%p0' -> switch modem power pin off");
    Serial.println(" '%r1' -> switch modem reset pin on");
    Serial.println(" '%r0' -> switch modem reset pin off");
    Serial.println(" '#<n>' -> execute cmd <n> from cmd list");
    Serial.println(" '#<n>-<m>' -> execute from cmd <n> to cmd <m> from cmd list");
    Serial.println(" '.' -> repeat last cmd");
    Serial.println(" '&1' -> add certificate 1");
    Serial.println(" '&2' -> add certificate 2");
    Serial.println(" '&3' -> add certificate 3");
    Serial.println(" '&4' -> add certificate 4");
    Serial.println(" '&5' -> add certificate 5");
    Serial.println(" '&6' -> add certificate 6");
    Serial.println(" '&' -> start / stop record mode");
    Serial.println(" '$' -> execute recorded cmd list");
    Serial.println(" '!' -> show recorded cmd list");
    Serial.println(" '§' -> show file list");
    Serial.println(" ':' -> show ssl configuration");
    Serial.println(" '++' -> additional cmd info");
    Serial.println(" '--' -> no additional cmd info");
    Serial.println(" '<cmd>[#<w>[x<n>]]'");
    Serial.println("   -> optional cmd parameter w of wait time for response in ms");
    Serial.println("   -> optional cmd parameter n of execution times");
    Serial.println("---- help ----------------------------------");
    listCmds();
    listRecords();
    listFiles();
    Serial.print("<"); Serial.print(restCmd); Serial.println(">");
    Serial.print("len:"); Serial.print(strlen(restCmd)); Serial.println(">");
}

void listSslCfg() {
    Serial.println("----SSL config ----------------------------------");
    cmdsToExecute.push_back("AT+QSSLCFG=\"sslversion\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"ciphersuite\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"cacert\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"clientcert\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"clientkey\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"seclevel\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"ignorelocaltime\",1");
    cmdsToExecute.push_back("AT+QSSLCFG=\"negotiatetime\",1");
    bool oldInfoMode = infoMode;
    infoMode = false;
    sendAndReceive();
    infoMode = oldInfoMode;
    Serial.println("----SSL config ----------------------------------");
}

void listFiles() {
    Serial.println("----file list ----------------------------------");
    cmdsToExecute.push_back("AT+QFLST");
    bool oldInfoMode = infoMode;
    infoMode = false;
    sendAndReceive();
    infoMode = oldInfoMode;
    Serial.println("----file list ----------------------------------");
}

void listCmds() {
    Serial.println("----cmd list ----------------------------------");
    for (unsigned int i = 0; i < cmds.size(); i++) {
        Serial.print("#"); Serial.print(i); Serial.print(" <"); Serial.print(cmds[i]); Serial.println(">");
    }
    Serial.println("----cmd list ----------------------------------");
}

void listRecords() {
    Serial.println("----records list ----------------------------------");
    for (unsigned int i = 0; i < cmdsRecorded.size(); i++) {
        Serial.print("#"); Serial.print(i); Serial.print(" <"); Serial.print(cmdsRecorded[i]); Serial.println(">");
    }
    Serial.println("----records list ----------------------------------");
}

bool recordMode = false;
void readInput() {
    while(Serial.available()>0) {
        String s = Serial.readString();
        if (s.startsWith("#")) {
            //#2
            //#2-5
            int fi = atoi(s.substring(1).c_str());
            int pe = s.indexOf("-");
            cmdsLastToExecute.clear();
            if (pe < 0) {
                cmdsToExecute.push_back(s);
                cmdsLastToExecute.push_back(s);
                if (recordMode) {
                    cmdsRecorded.push_back(s);
                }
            } else {
                int li = atoi(s.substring(pe + 1).c_str());
                for (int i = fi; i <= li; i++) {
                    cmdsToExecute.push_back(String("#") + String(i));
                    cmdsLastToExecute.push_back(String("#") + String(i));
                    if (recordMode) {
                        cmdsRecorded.push_back(String("#") + String(i));
                    }
                }
            }
        } else if (s.startsWith(".")) {
            for (unsigned int i = 0; i < cmdsLastToExecute.size(); i++) {
                cmdsToExecute.push_back(cmdsLastToExecute[i]);
            }
        } else if (s.startsWith("?")) {
            showHelp();
        } else if (s.startsWith("&&")) {
            appendDataToFile("cert.pem", baltimoreCyberTrustRootCA);
            appendDataToFile("cert.pem", teleSecGlobalRootCA);
            appendDataToFile("cert.pem", teleSecServerPassCA2);
            appendDataToFile("cert.pem", wwwgooglecom);
            appendDataToFile("cert.pem", sigRoCAR2);
            appendDataToFile("cert.pem", goAutG3);
        } else if (s.startsWith("&1")) {
            appendDataToFile("cert.pem", baltimoreCyberTrustRootCA);
        } else if (s.startsWith("&2")) {
            appendDataToFile("cert.pem", teleSecGlobalRootCA);
        } else if (s.startsWith("&3")) {
            appendDataToFile("cert.pem", teleSecServerPassCA2);
        } else if (s.startsWith("&4")) {
            appendDataToFile("cert.pem", wwwgooglecom);
        } else if (s.startsWith("&5")) {
            appendDataToFile("cert.pem", sigRoCAR2);
        } else if (s.startsWith("&6")) {
            appendDataToFile("cert.pem", goAutG3);
        } else if (s.startsWith("&")) {
            if (!recordMode) {
                Serial.println("# start recording");
                cmdsRecorded.clear();
            } else {
                Serial.println("# stop recording");
                listRecords();
            }
            recordMode = !recordMode;
        } else if (s.startsWith("$")) {
            for (unsigned int i = 0; i < cmdsRecorded.size(); i++) {
                cmdsToExecute.push_back(cmdsRecorded[i]);
            }
        } else if (s.startsWith("!")) {
            listRecords();
        } else if (s.startsWith("§")) {
            listFiles();
        } else if (s.startsWith(":")) {
            listSslCfg();
        } else if (s.startsWith("++")) {
            infoMode = true;
        } else if (s.startsWith("--")) {
            infoMode = false;
        } else if (s.startsWith("%p1")) {
            modemPower(true);
        } else if (s.startsWith("%p0")) {
            modemPower(false);
        } else if (s.startsWith("%r1")) {
            modemReset(true);
        } else if (s.startsWith("%r0")) {
            modemReset(false);
        } else {
            cmdsToExecute.push_back(s);
            cmdsLastToExecute.clear();
            cmdsLastToExecute.push_back(s);
            if (recordMode) {
                cmdsRecorded.push_back(s);
            }
        }
    }
}

void appendDataToFile(const char* filename, String data) {
    Serial.print("appendDataToFile <"); Serial.print(filename); Serial.print("> <"); Serial.print(data.length()); Serial.println("> byte...");
    cmdsToExecute.push_back(String("AT+QFOPEN=") + filename + ",0");
    cmdsToExecute.push_back("AT+QFSEEK=2,0,2");
    int s = 0;
    int e = data.length();
    int l;
    while(e > 0) {
        l = e - s;
        if (l > 1024) {
            l = 1024;
        } else {
            e = 0;
        }
        cmdsToExecute.push_back(String("AT+QFWRITE=2,") + l);
        cmdsToExecute.push_back(data.substring(s, s + l));
        s += l;
    }
    cmdsToExecute.push_back("AT+QFCLOSE=2");
}



void initCmds() {
    if (!cmds.size()) {

        cmds.push_back("AT+QIOPEN=1,0,\"TCP\",\"220.180.239.212\",8009,0,0#10000");
        cmds.push_back("AT+QISTATE#1000");
        cmds.push_back("AT+QIRD=0,0#1000");
        cmds.push_back("AT+QISEND=0,10#1000");
        cmds.push_back("1234567890");
        cmds.push_back("AT+QIRD=0,0#1000");
        cmds.push_back("AT+QIRD=0,3#1000");
        cmds.push_back("AT+QIRD=0,0#1000");
        cmds.push_back("AT+QIRD=0,7#1000");
        cmds.push_back("AT+QIRD=0,0#1000");
        cmds.push_back("AT+QICLOSE=0#1000");
        cmds.push_back("");

        cmds.push_back("AT+CCLK=\"18/07/27,00:00:00+00\"");
        cmds.push_back("AT+CFUN=1,1#15000");
        cmds.push_back("AT+COPS=1#1000");
        cmds.push_back("AT+COPS?");
        cmds.push_back("AT+CSQ");
        cmds.push_back("AT+QIDEACT=1#1000");
        cmds.push_back("AT+QICSGP=1,1,\"internet.t-mobile\",\"tm\",\"tm\",1#1000");
        cmds.push_back("AT+QIACT=1#5000");
        cmds.push_back("AT+QIACT?#1000");
        cmds.push_back("");

        cmds.push_back("AT+CCLK=\"18/07/27,00:00:00+00\"");
        cmds.push_back("AT+CFUN=1,1#15000");
        cmds.push_back("AT+COPS=1#1000");
        cmds.push_back("AT+COPS?");
        cmds.push_back("AT+CSQ");
        cmds.push_back("AT+QIDEACT=1#1000");
        cmds.push_back("AT+QICSGP=1,1,\"iot.1nce.net\",\"1n\",\"1n\",1#1000");
        cmds.push_back("AT+QIACT=1#5000");
        cmds.push_back("AT+QIACT?#1000");
        cmds.push_back("");

        cmds.push_back("AT+QSSLCFG=\"sslversion\",1,3#1000");
        cmds.push_back("AT+QSSLCFG=\"seclevel\",1,1#1000");
        cmds.push_back("AT+QSSLCFG=\"ciphersuite\",1,0X002F#1000");
        cmds.push_back("AT+QSSLCFG=\"cacert\",1,\"UFS:cert.pem\"");
        cmds.push_back("");

        cmds.push_back("AT+QSSLOPEN=1,1,0,\"nb-iot.ram.m2m.telekom.com\",8883,0#3000");
        cmds.push_back("AT+QSSLOPEN=1,1,0,\"217.74.206.151\",8883,0#3000");
        cmds.push_back("AT+QSSLSTATE#1000x60");
        cmds.push_back("AT+QSSLSTATE#1000");
        cmds.push_back("AT+QSSLSEND=0,0#5000");
        cmds.push_back("1234");
        cmds.push_back("AT+QSSLCLOSE=0");
        cmds.push_back("");

        cmds.push_back("AT+QFDEL=\"cert.pem\"");
        cmds.push_back("AT+QFDEL=\"*\"");
        cmds.push_back("");
        cmds.push_back("AT+QFOPEN=cert.pem,0");
        cmds.push_back("AT+QFREAD=2#5000");
        cmds.push_back("AT+QFCLOSE=2");
        cmds.push_back("AT+QFLST");
        cmds.push_back("AT+QFLDS=\"UFS\"");
        cmds.push_back("");

        cmds.push_back("AT+CCLK=\"18/08/20,12:00:00+00\"");
        cmds.push_back("AT+CFUN=1,1#15000");
        cmds.push_back("AT+COPS=1#5000");
        cmds.push_back("AT+COPS?");
        cmds.push_back("AT+CSQ");
        cmds.push_back("AT+QIDEACT=1#1000");
        cmds.push_back("AT+QICSGP=1,1,\"iot.1nce.net\",\"1n\",\"1n\",1#1000");
        cmds.push_back("AT+QIACT=1#5000");
        cmds.push_back("AT+QIACT?#1000");
        cmds.push_back("AT+QSSLOPEN=1,1,0,\"cti.ram.m2m.telekom.com\",443,0#3000");
        cmds.push_back("AT+QSSLSEND=0,10");
        cmds.push_back("1234567890");
        cmds.push_back("AT+QSSLRECV=0,64");
        cmds.push_back("AT+QIGETERROR");
        cmds.push_back("AT+QSSLCLOSE=0");
		cmds.push_back(restCmd);

    }
}
