const int sensorPin = A0;
const int buzzerPin = 2;
const int ledPin = 3;

int zeroOffset = 0; 
// --- ตั้งค่าเกณฑ์ 2 ระดับ ---
int thresholdWarning = 5;  // เกณฑ์ระดับอ่อน5
int thresholdDanger = 10;  // เกณฑ์ระดับชัดเจน15

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // --- เพิ่มเสียงปี๊บ 3 ครั้ง เมื่อเริ่มจ่ายไฟ ---
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH); 
    digitalWrite(ledPin, HIGH);    
    delay(150);                    
    digitalWrite(buzzerPin, LOW);  
    digitalWrite(ledPin, LOW);     
    delay(150);                    
  }
  
  delay(500); 
  // ----------------------------------------

  // --- ช่วง Auto Calibration ---
  Serial.println("Calibrating... Keep probe still.");
  long sum = 0;
  for(int i = 0; i < 500; i++) {
    sum += analogRead(sensorPin);
    delay(2);
  }
  zeroOffset = sum / 500; 
  Serial.print("New Zero Offset: ");
  Serial.println(zeroOffset);
  // -----------------------------
}

void loop() {
  int maxDiff = 0;
  for (int i = 0; i < 100; i++) {
    int val = analogRead(sensorPin);
    int diff = abs(val - zeroOffset); 
    if (diff > maxDiff) maxDiff = diff;
    delay(1);
  }

  // พิมพ์เฉพาะตัวเลข เผื่อต้องการนำกลับไปเสียบดูด้วย Serial Plotter
  Serial.println(maxDiff);

  // --- เงื่อนไขการแจ้งเตือน 2 ระดับ ---
  if (maxDiff > thresholdDanger) {
    // ระดับที่ 2: พบไฟรั่วชัดเจน (maxDiff > 15)
    digitalWrite(ledPin, HIGH);    // LED ติดค้าง
    digitalWrite(buzzerPin, HIGH); // Buzzer เตือนสลับกันความร้อนสะสม
    delay(100); 
    digitalWrite(buzzerPin, LOW);
    delay(100); 
    
  } else if (maxDiff > thresholdWarning) {
    // ระดับที่ 1: พบไฟรั่วระดับอ่อน (maxDiff > 5 แต่ไม่เกิน 15)
    digitalWrite(buzzerPin, LOW);  // มั่นใจว่าปิดเสียงอยู่
    digitalWrite(ledPin, HIGH);    // สั่ง LED ติด
    delay(300);                    // ค้างไว้ 0.3 วิ
    digitalWrite(ledPin, LOW);     // สั่ง LED ดับ
    delay(300);                    // ค้างไว้ 0.3 วิ (เกิดการกระพริบช้าๆ)
    
  } else {
    // ปลอดภัย (maxDiff <= 5)
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW); 
  }
}