#include <Fuzzy.h>
#include <DHT.h>

#define DHT_PIN 4      
#define DHT_TYPE DHT22  
#define FAN_PIN 9      

DHT dht(DHT_PIN, DHT_TYPE);

Fuzzy *fuzzy = new Fuzzy();

FuzzySet *near = new FuzzySet(0, 20, 20, 40);
FuzzySet *safe = new FuzzySet(30, 50, 50, 70);
FuzzySet *distant = new FuzzySet(60, 80, 100, 100);

FuzzySet *stopedInput = new FuzzySet(0, 0, 0, 0);
FuzzySet *slowInput = new FuzzySet(1, 10, 10, 20);
FuzzySet *normalInput = new FuzzySet(15, 30, 30, 50);
FuzzySet *quickInput = new FuzzySet(45, 60, 70, 70);

FuzzySet *cold = new FuzzySet(-30, -30, -20, -10);
FuzzySet *good = new FuzzySet(-15, 0, 0, 15);
FuzzySet *hot = new FuzzySet(10, 20, 30, 30);

FuzzySet *dry = new FuzzySet(0, 20, 20, 40);
FuzzySet *normalHumidity = new FuzzySet(30, 50, 50, 70);
FuzzySet *highHumidity = new FuzzySet(60, 80, 80, 100);

FuzzySet *minimum = new FuzzySet(0, 20, 20, 40);
FuzzySet *average = new FuzzySet(30, 50, 50, 70);
FuzzySet *maximum = new FuzzySet(60, 80, 80, 100);

FuzzySet *stopedOutput = new FuzzySet(0, 0, 0, 0);
FuzzySet *slowOutput = new FuzzySet(1, 10, 10, 20);
FuzzySet *normalOutput = new FuzzySet(15, 30, 30, 50);
FuzzySet *quickOutput = new FuzzySet(45, 60, 70, 70);

FuzzySet *lowSpeed = new FuzzySet(0, 30, 30, 60);
FuzzySet *mediumSpeed = new FuzzySet(40, 70, 70, 100);
FuzzySet *highSpeed = new FuzzySet(80, 100, 100, 100);

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));

  FuzzyInput *distance = new FuzzyInput(1);
  distance->addFuzzySet(near);
  distance->addFuzzySet(safe);
  distance->addFuzzySet(distant);
  fuzzy->addFuzzyInput(distance);

  FuzzyInput *speedInput = new FuzzyInput(2);
  speedInput->addFuzzySet(stopedInput);
  speedInput->addFuzzySet(slowInput);
  speedInput->addFuzzySet(normalInput);
  speedInput->addFuzzySet(quickInput);
  fuzzy->addFuzzyInput(speedInput);

  FuzzyInput *temperature = new FuzzyInput(3);
  temperature->addFuzzySet(cold);
  temperature->addFuzzySet(good);
  temperature->addFuzzySet(hot);
  fuzzy->addFuzzyInput(temperature);


  FuzzyInput *humidity = new FuzzyInput(4);
  humidity->addFuzzySet(dry);
  humidity->addFuzzySet(normalHumidity);
  humidity->addFuzzySet(highHumidity);
  fuzzy->addFuzzyInput(humidity);


  FuzzyOutput *risk = new FuzzyOutput(1);
  risk->addFuzzySet(minimum);
  risk->addFuzzySet(average);
  risk->addFuzzySet(maximum);
  fuzzy->addFuzzyOutput(risk);

  FuzzyOutput *speedOutput = new FuzzyOutput(2);
  speedOutput->addFuzzySet(stopedOutput);
  speedOutput->addFuzzySet(slowOutput);
  speedOutput->addFuzzySet(normalOutput);
  speedOutput->addFuzzySet(quickOutput);
  fuzzy->addFuzzyOutput(speedOutput);

  FuzzyOutput *fanSpeed = new FuzzyOutput(3);
  fanSpeed->addFuzzySet(lowSpeed);
  fanSpeed->addFuzzySet(mediumSpeed);
  fanSpeed->addFuzzySet(highSpeed);
  fuzzy->addFuzzyOutput(fanSpeed);

  FuzzyRuleAntecedent *distanceNearAndSpeedQuick = new FuzzyRuleAntecedent();
  distanceNearAndSpeedQuick->joinWithAND(near, quickInput);
  FuzzyRuleAntecedent *temperatureCold = new FuzzyRuleAntecedent();
  temperatureCold->joinSingle(cold);
  FuzzyRuleAntecedent *ifDistanceNearAndSpeedQuickOrTemperatureCold = new FuzzyRuleAntecedent();
  ifDistanceNearAndSpeedQuickOrTemperatureCold->joinWithOR(distanceNearAndSpeedQuick, temperatureCold);

  FuzzyRuleConsequent *thenRiskMaximumAndSpeedSlow = new FuzzyRuleConsequent();
  thenRiskMaximumAndSpeedSlow->addOutput(maximum);
  thenRiskMaximumAndSpeedSlow->addOutput(slowOutput);

  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifDistanceNearAndSpeedQuickOrTemperatureCold, thenRiskMaximumAndSpeedSlow);
  fuzzy->addFuzzyRule(fuzzyRule1);

  FuzzyRuleAntecedent *distanceSafeAndSpeedNormal = new FuzzyRuleAntecedent();
  distanceSafeAndSpeedNormal->joinWithAND(safe, normalInput);
  FuzzyRuleAntecedent *ifDistanceSafeAndSpeedNormalOrTemperatureGood = new FuzzyRuleAntecedent();
  ifDistanceSafeAndSpeedNormalOrTemperatureGood->joinWithOR(distanceSafeAndSpeedNormal, good);

  FuzzyRuleConsequent *thenRiskAverageAndSpeedNormal = new FuzzyRuleConsequent();
  thenRiskAverageAndSpeedNormal->addOutput(average);
  thenRiskAverageAndSpeedNormal->addOutput(normalOutput);

  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifDistanceSafeAndSpeedNormalOrTemperatureGood, thenRiskAverageAndSpeedNormal);
  fuzzy->addFuzzyRule(fuzzyRule2);

  FuzzyRuleAntecedent *distanceDistantAndSpeedSlow = new FuzzyRuleAntecedent();
  distanceDistantAndSpeedSlow->joinWithAND(distant, slowInput);
  FuzzyRuleAntecedent *ifDistanceDistantAndSpeedSlowOrTemperatureHot = new FuzzyRuleAntecedent();
  ifDistanceDistantAndSpeedSlowOrTemperatureHot->joinWithOR(distanceDistantAndSpeedSlow, hot);

  FuzzyRuleConsequent *thenRiskMinimumSpeedQuick = new FuzzyRuleConsequent();
  thenRiskMinimumSpeedQuick->addOutput(minimum);
  thenRiskMinimumSpeedQuick->addOutput(quickOutput);

  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifDistanceDistantAndSpeedSlowOrTemperatureHot, thenRiskMinimumSpeedQuick);
  fuzzy->addFuzzyRule(fuzzyRule3);
}

void loop()
{
  int input1 = random(0, 100);
  int input2 = random(0, 70);
  int input3 = random(-30, 30);

  Serial.println("\n\n\nEntrance: ");
  Serial.print("\t\t\tDistance: ");
  Serial.print(input1);
  Serial.print(", Speed: ");
  Serial.print(input2);
  Serial.print(", Temperature: ");
  Serial.print(input3);

  float input4 = dht.readHumidity();
  fuzzy->setInput(1, input1);
  fuzzy->setInput(2, input2);
  fuzzy->setInput(3, input3);
  fuzzy->setInput(4, input4);

  fuzzy->fuzzify();

  Serial.println("Input: ");
  Serial.print("\tDistance: Near-> ");
  Serial.print(near->getPertinence());
  Serial.print(", Safe-> ");
  Serial.print(safe->getPertinence());
  Serial.print(", Distant-> ");
  Serial.println(distant->getPertinence());

  Serial.print("\tSpeed: Stoped-> ");
  Serial.print(stopedInput->getPertinence());
  Serial.print(", Slow-> ");
  Serial.print(slowInput->getPertinence());
  Serial.print(", Normal-> ");
  Serial.print(normalInput->getPertinence());
  Serial.print(", Quick-> ");
  Serial.println(quickInput->getPertinence());

  Serial.print("\tTemperature: Cold-> ");
  Serial.print(cold->getPertinence());
  Serial.print(", Good-> ");
  Serial.print(good->getPertinence());
  Serial.print(", Hot-> ");
  Serial.println(hot->getPertinence());

  Serial.print("\tHumidity: Dry-> ");
  Serial.print(dry->getPertinence());
  Serial.print(", Normal-> ");
  Serial.print(normalHumidity->getPertinence());
  Serial.print(", High-> ");
  Serial.println(highHumidity->getPertinence());

  float output1 = fuzzy->defuzzify(1);
  float output2 = fuzzy->defuzzify(2);
  float output3 = fuzzy->defuzzify(3);

  analogWrite(FAN_PIN, output3);

  Serial.println("Output: ");
  Serial.print("\tRisk: Minimum-> ");
  Serial.print(minimum->getPertinence());
  Serial.print(", Average-> ");
  Serial.print(average->getPertinence());
  Serial.print(", Maximum-> ");
  Serial.println(maximum->getPertinence());

  Serial.print("\tSpeed: Stoped-> ");
  Serial.print(stopedOutput->getPertinence());
  Serial.print(", Slow-> ");
  Serial.print(slowOutput->getPertinence());
  Serial.print(", Normal-> ");
  Serial.print(normalOutput->getPertinence());
  Serial.print(", Quick-> ");
  Serial.println(quickOutput->getPertinence());

  Serial.print("\tFan Speed: Low-> ");
  Serial.print(lowSpeed->getPertinence());
  Serial.print(", Medium-> ");
  Serial.print(mediumSpeed->getPertinence());
  Serial.print(", High-> ");
  Serial.println(highSpeed->getPertinence());

  Serial.println("Result: ");
  Serial.print("\tRisk: ");
  Serial.print(output1);
  Serial.print(", Speed: ");
  Serial.print(output2);
  Serial.print(", Fan Speed: ");
  Serial.println(output3);

  delay(12000);
}

