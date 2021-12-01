#include "disco_kokomo.h"

// music player
Adafruit_VS1053_FilePlayer kokomo_player(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);
const char kSong[] = "/kokomo.mp3";

// stepper motor
// default constructor for now
AccelStepper disco_ball; // AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

// led spotlights
// instantiate one jewel for now
Adafruit_NeoPixel spotlight(LED_COUNT, LED_DATA_PIN, NEO_RGBW + NEO_KHZ800);
const uint32_t kWhite = spotlight.Color(0, 0, 0, 64);

// global flags
volatile uint8_t door_event = 0;
volatile DoorState state = DoorState::kUnknown;

void DoorCallback()
{
    door_event = 1;
    state = GetDoorState(DOOR_SENSOR_PIN);
}

void setup()
{
    Serial.begin(115200);

    // init music player
    if(!kokomo_player.begin())
    {
        Serial.println(F("Couldn't find VS1053, check pins."));
    }
    if(!SD.begin(CARDCS))
    {
        Serial.println(F("SD failed or not present."));
    }
    kokomo_player.setVolume(10,10);
    kokomo_player.useInterrupt(VS1053_FILEPLAYER_PIN_INT); // use DREQ int

    // init spotlight
    spotlight.begin();
    spotlight.show();
    spotlight.setBrightness(64);

    // disco ball motor setup
    disco_ball.setMaxSpeed(RpmToSpeed(6.0)); // 6 rpm is probably the fastest the disco ball should spin
    disco_ball.setSpeed(RpmToSpeed(2.0)); // avg speed for disco ball is 2-3 rpm, 1-1.5 rpm for more drama

    // door interrupts
    attachInterrupt(digitalPinToInterrupt(DOOR_SENSOR_PIN), DoorCallback, CHANGE); // use door callback on any edge (i.e. door open and close)
    EIC->CONFIG[1].bit.FILTEN0 = 1; // enable 3 sample, majorit vote filter on EXTINT8

    // get starting door state
    state = GetDoorState(DOOR_SENSOR_PIN);
}

void loop()
{
    if(door_event)
    {
        // when door is closed for the first time
        if(state == DoorState::kClosed)
        {
            kokomo_player.stopPlaying();
            spotlight.clear();
            spotlight.show();
            disco_ball.stop();
        }
        door_event = 0;
    }

    switch(state)
    {
    case DoorState::kOpen:
        if(kokomo_player.stopped())
        {
            kokomo_player.startPlayingFile(kSong); // start song in background
        }
        if(spotlight.getPixelColor(0) != kWhite)
        {
            spotlight.fill(kWhite, 0);
            spotlight.show();
        }
        disco_ball.runSpeed(); // call as frequently as possible
        break;
    case DoorState::kClosed:
        break;
    case DoorState::kUnknown:
    default:
        // we shouldn't get here
        break;
    }
}