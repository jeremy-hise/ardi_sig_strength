
#include <XBee.h>                  //http://code.google.com/p/xbee-arduino/
#include <Streaming.h>             //http://arduiniana.org/libraries/streaming/

//XBee variables
XBee xbee = XBee();                //XBee object
ZBRxResponse rx = ZBRxResponse();  //XBee receive response

void setup(void)
{
    Serial.begin(115200);
}

void loop(void)
{
    xbee.readPacket();

    if (xbee.getResponse().isAvailable()) {

        switch (xbee.getResponse().getApiId()) {           //what kind of packet did we get?
    
        case ZB_RX_RESPONSE:                               //rx data packet
            xbee.getResponse().getZBRxResponse(rx);        //get the received data
            switch (rx.getOption()) {
            case ZB_PACKET_ACKNOWLEDGED:
                Serial << "Packet received, RSS = -" << _DEC(rss()) << " DB" << endl;
                break;
            }
        }
    }
}

/*-----------------------------------------------------------------------*
* returns received signal strength value for the last RF data packet.   *
*-----------------------------------------------------------------------*/
uint8_t rss() {

    uint8_t atCmd[] = {'D', 'B'}, respLen, *resp, dBm;
    AtCommandRequest atCmdReq;
    AtCommandResponse atResp;

    atCmdReq = AtCommandRequest(atCmd);

    xbee.send(atCmdReq);
    if (xbee.readPacket(10)) {
        if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
            xbee.getResponse().getAtCommandResponse(atResp);
            if (atResp.isOk()) {
                respLen = atResp.getValueLength();
                if (respLen == 1) {
                    resp = atResp.getValue();
                    dBm = resp[0];
                    return dBm;
                }
                else {
                    Serial << "RSS LEN ERR" << endl;    //unexpected length
                }
            }
            else {
                Serial << "RSS ERR" << endl;            //status not ok
            }
        }
        else {
            Serial << "RSS UNEXP RESP" << endl;         //expecting AT_COMMAND_RESPONSE, got something else
        }
    }
    else {
        Serial << "RSS NO RESP" << endl;                //timed out
    }
}
