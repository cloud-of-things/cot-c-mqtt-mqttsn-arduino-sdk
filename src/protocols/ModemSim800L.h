/*
 * ModemSim800L.h
 *
 *  Created on: 19.06.2018
 *      Author: hhenkens
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_MODEMSIM800L_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_MODEMSIM800L_H_

#include <protocols/DTCoTModemBase.h>
#include "utility/DTCoTDebugOutput.h"

//namespace DTCoT {

class ModemSim800L: public DTCoTModemBase {
public:

	ModemSim800L(Stream & serial, long serialSpeed, int resetPin, void (*callback)()) :
            DTCoTModemBase(serial, serialSpeed, resetPin, callback) {
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::ModemSim800L()... done.");
    }

    ~ModemSim800L() {
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::~ModemSim800L()... done.");
    }

    DTCOT_MODEM_STATUS init() {
        DT_DEBUG_PRINTLN_DEBUG("ModemSim800L::init()...");
        DTCOT_MODEM_STATUS init_status =
                (DTCOT_MODEM_STATUS) DTCoTTcpIoTHardware_init(*stream, resetPin,
                        callback);
        if (init_status == DTCOT_MODEM_OK) {
            init_status = _atEcho(false);
            if (init_status == DTCOT_MODEM_OK) {
                String response;
                init_status = _atCommTest(response);
            }
        }
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::init()... done. <%d>", init_status);
        return init_status;
    }

    DTCOT_MODEM_STATUS radioOn(bool on) {
        String cmd = String("AT+CFUN=") + String(on) + "\r";
        _sendAndParse("radioOn/Off", cmd, 15000L);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS reconnectGprs() {
        if (DTCOT_MODEM_GPRS_CONNECTED == isGprsConnected()) {
            DT_DEBUG_PRINTLN_INFO(
                    "ModemSim800L::(re)connectGprs()... GPRS already connected");
            return DTCOT_MODEM_OK;
        }

        String sslCmdDescr = "Disable SSL";
        if (_ssl) {
            sslCmdDescr = "Enable SSL";
        }
        String sslCmd = "AT+CIPSSL=" + String(_ssl) + "\r";

        tmpRc = gprsDisconnect();
        if (tmpRc == DTCOT_MODEM_OK
                && _sendAndParse("Set the Bearer for the IP", "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", 0, 60000)
                && _sendAndParse("Set the APN", String("AT+SAPBR=3,1,\"APN\",\"") + _apnName + "\"\r")
                && _sendAndParse("Set the User", String("AT+SAPBR=3,1,\"USER\",\"") + apnUser + "\"\r")
                && _sendAndParse("Set the Password", String("AT+SAPBR=3,1,\"PWD\",\"") + apnPwd + "\"\r")
                && _sendAndParse("Define the PDP context", String("AT+CGDCONT=1,\"IP\",\"") + _apnName + "\"\r")
                && _sendAndParse("Activate the PDP context", "AT+CGACT=1,1\r", 4000, 60000)
                && _sendAndParse("Open the definied GPRS bearer context", "AT+SAPBR=1,1\r", 0, 85000)
                && _sendAndParse("Query the GPRS bearer context status", "AT+SAPBR=2,1\r", 0, 30000)
                && _sendAndParse("Attach to GPRS", "AT+CGATT=1\r", 0, 60000)
                && _sendAndParse(sslCmdDescr.c_str(), sslCmd, 0, 60000)
                && _sendAndParse("Set to multi-IP", "AT+CIPMUX=1\r")
                && _sendAndParse("Put in no 'quick send' mode", "AT+CIPQSEND=0\r")
                && _sendAndParse("Set to get data manually", "AT+CIPRXGET=1\r")
                && _sendAndParse("Start Task and Set APN, USER NAME, PASSWORD", String("AT+CSTT=\"") + _apnName + String("\",\"") + apnUser
                                + String("\",\"") + apnPwd + "\"\r", 0, 60000)
                && _sendAndParse("Bring Up Wireless Connection with GPRS or CSD", "AT+CIICR\r", 0, 60000)
                && _sendAndParse("Get Local IP Address", "AT+CIFSR;E0\r", 0, 10000)
                && _sendAndParse("Configure Domain Name Server (DNS)", "AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"\r")) {
        };
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::(re)connectGprs()... done. <%d>",
                tmpRc);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS isGprsConnected() {
        _sendAndParse("isGprsConnected", "AT+CGATT?\r", 0, 10000, true, ".*+CGATT: (.).*");
        if (_dummyResponse.equals("1")) {
            return DTCOT_MODEM_GPRS_CONNECTED;
        }
        return DTCOT_MODEM_GPRS_NOT_CONNECTED;
    }

    DTCOT_MODEM_STATUS gprsDisconnect() {
        if (_sendAndParse("gprsDisconnect 1/2", "AT+CGATT=0\r", 0, 60000)
                && _sendAndParse("gprsDisconnect 2/2", "AT+CIPSHUT\r", 0, 60000, true,
                        "(.*)\r\nSHUT OK")) {
        }
        return tmpRc;
    }

    DTCOT_MODEM_STATUS connectGprs(const char* apn, const char* user = NULL, const char* pwd = NULL, int ssl = 1) {
        _apnName = apn;
        apnUser = user;
        apnPwd = pwd;
        _ssl = ssl;
        return reconnectGprs();
    }

    DTCOT_MODEM_STATUS isSocketConnected() {
        if (_socket_num == DTCOT_MODEM_BASE_TCP_INVALID_SOCKET) {
            return DTCOT_MODEM_TCP_NOT_CONNECTED;
        }
        return DTCOT_MODEM_TCP_CONNECTED;
    }

    DTCOT_MODEM_STATUS connectSocket(String host, int port, int mux = 1) {
        DT_DEBUG_PRINTLN_DEBUG("ModemSim800L::connectSocket() host<%s> port<%d> mux<%d>...",
                host.c_str(), port, mux);
        tmpRc = isSocketConnected();
        if (DTCOT_MODEM_TCP_CONNECTED == tmpRc && _socket_host.equals(host)
                && _port == port && _mux == mux) {
            DT_DEBUG_PRINTLN_DEBUG("ModemSim800L::connectSocket()... already connected!");
            return tmpRc;
        }
        if (DTCOT_MODEM_TCP_CONNECTED == tmpRc) {
            closeSocket();
        }
        reconnectGprs();
        _socket_host = host;
        _port = port;
        _mux = mux;
        String gmxSerialString;
        _socket_num = DTCOT_MODEM_BASE_TCP_INVALID_SOCKET;
        if (_sendAndParse("connectSocket", String("AT+CIPSTART=") + String(mux) + String(",\"TCP\",\"") + host + String("\",") + String(port) + "\r",
        		0, 75000, true, ".*\r\n(.*), CONNECT OK")) {
            _socket_num = atoi(_dummyResponse.c_str());
            tmpRc = DTCOT_MODEM_TCP_CONNECTED;
        }
        DT_DEBUG_PRINTLN_INFO(
                "ModemSim800L::connectSocket() host<%s> port<%d> mux<%d>... done. socket_num<%d> rc<%d>",
                _socket_host.c_str(), _port, _mux, _socket_num, tmpRc);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS closeSocket() {
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::closeSocket()...");
        tmpRc = isSocketConnected();
        if (DTCOT_MODEM_TCP_NOT_CONNECTED == tmpRc) {
            DT_DEBUG_PRINTLN_INFO("ModemSim800L::closeSocket()... already closed!");
            return DTCOT_MODEM_OK;
        }
        String atCmd = String("AT+CIPCLOSE=") + String(_socket_num) + "\r";
        String regexp = String("(.*)\r\n") + String(_socket_num) + ", CLOSE OK";
        if (_sendAndParse("closeSocket", atCmd, 0, 75000, true, regexp.c_str())) {
        }
        _socket_num = DTCOT_MODEM_BASE_TCP_INVALID_SOCKET;
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::closeSocket()... done. <%d>", tmpRc);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS getVersion(String& version) {
        _sendAndParse("getVersion", "AT+CGMR\r", version);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS getICCID(String& iccid) {
        _sendAndParse("getICCID", "AT+CCID\r", iccid);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS getIMEI(String& imei) {
        _sendAndParse("getIMEI", "AT+CGSN\r", imei);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS getIMSI(String& imsi) {
        _sendAndParse("getIMSI", "AT+CIMI\r", imsi);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS getCSQ(String& csq) {
        _sendAndParse("getCSQ", "AT+CSQ\r", csq);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS getLocalIp(String& localIp) {
        _sendAndParse("getLocalIp", "AT+CIFSR\r", localIp,
                0, 2000, true, "(.*)");
        return tmpRc;
    }

    DTCOT_MODEM_STATUS txData(const uint8_t *buf, size_t size) {
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::txData() uint8_t[%d]...", size);
        String cmd = String("AT+CIPSEND=") + String(_socket_num) + String(",")
                + String(size) + "\r";
        String sendOkPattern = String(".*") + String(_socket_num)
                + ", (SEND OK).*\r\n";
        //no quick send mode
        if (_sendAndParse("gmxTcp_TXData", cmd, 0, 75000, true, ".*(>)")
                && _sendAndParse("TX", buf, size, 0, 5000, true, sendOkPattern.c_str())) {
            if (tmpSerialString.indexOf(", CLOSE") > -1) {
                DT_DEBUG_PRINTLN_INFO(
                        "ModemSim800L::txData closed socket detected!");
                _socket_num = DTCOT_MODEM_BASE_TCP_INVALID_SOCKET;
            }
        }
        DT_DEBUG_PRINTLN_INFO("ModemSim800L::txData() uint8_t[%d]... done. <%d>",
                size, tmpRc);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS rxDataAvailable(int &available) {
        String cmd = String("AT+CIPRXGET=4,") + String(_socket_num) + "\r";
        if (_sendAndParse("gmxTcp_Available", cmd, 0, 3000, true, ".*CIPRXGET:.*,.*,(.*)\r\n")) {
            if (tmpSerialString.indexOf(", CLOSE") > -1) {
                DT_DEBUG_PRINTLN_INFO(
                        "ModemSim800L::available closed socket detected!");
                _socket_num = DTCOT_MODEM_BASE_TCP_INVALID_SOCKET;
            }
            available = atoi(_dummyResponse.c_str());
        }
        DT_DEBUG_PRINTLN_INFO(
                "ModemSim800L::rxDataAvailable()... done. socket<%d> available<%d> <%d>",
                _socket_num, available, tmpRc);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS rxData(uint8_t *buf, int &size) {
        String cmd = String("AT+CIPRXGET=2,") + String(_socket_num) + String(",")
                + String(size) + "\r";
        if (_sendAndParse("gmxTcp_RXData", cmd, 0, 10000, true,
                ".*+CIPRXGET:[^\r\n]*\r\n(.*)\r\nOK")) {
            _dummyResponse.toCharArray((char*) buf, size);
            return DTCOT_MODEM_OK;
        }
        return DTCOT_MODEM_WRONG_SIZE;
    }



private:

    DTCOT_MODEM_STATUS _atCommTest(String& response) {
        _sendAndParse("AtCommTest", "AT\r", response);
        return tmpRc;
    }

    DTCOT_MODEM_STATUS _atEcho(bool on) {
        _sendAndParse("AtEcho", String("ATE") + String(on) + "\r");
        return tmpRc;
    }
};

//} /* namespace DTCoT */

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_MODEMSIM800L_H_ */
