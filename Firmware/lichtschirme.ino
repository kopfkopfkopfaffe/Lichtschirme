int TX = 1;
int DMX_RE = 2;
int DMX_DE = 4;
int channel = 0;
bool break_received = false;
int status;
int data;
int predefined = 0;
void setup()
{
  pinMode(DMX_DE, OUTPUT);
  pinMode(DMX_RE, OUTPUT);
  // configure serial port for DMX
  UBRR0H = 0;
  UBRR0L = 7;           // 250k baud
  UCSR0A = (1<<U2X0);
  UCSR0B = (1<<RXEN0);
  UCSR0C = (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);//|(1<<URSEL);

  digitalWrite(DMX_RE, 0);
  digitalWrite(DMX_DE, 0);
  analogWrite(9, 255); // Initial state is off = high
  analogWrite(10, 255); // Initial state is off = high
  analogWrite(11, 255); // Initial state is off = high
}

void loop()
{
  while (!(UCSR0A & (1<<RXC0))) { }
  status = UCSR0A;
  data = UDR0;

  if (status & (1<<FE0)) { // This is probably a break
    break_received = true;
    channel = 0;
  }
  else if (status & (1<<DOR0)) { // A real error occured (started mid-frame?)
    break_received = false;
  }
  else if (break_received) { 
    if (channel == 0) {
      if (data != 0) { // Assuming the first frame is 0
        delayMicroseconds(10);
        break_received = false;
      }
      else {
        channel++;
      }
    }
    else {
      if (predefined == 0) {
        if (channel == 51) {
          analogWrite(5, data);
        }
        else if (channel == 52) {
          analogWrite(6, data);
        }
        else if (channel == 53) {
          analogWrite(3, data);
        }
      }
      else if (channel == 54) {
        predefined = data;
      }
      channel++;
    }
  }
}
