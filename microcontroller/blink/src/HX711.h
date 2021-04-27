
int HX711_Read(void);
int HX711_GetOffset(void);
void HX711_init();
void Delay(int val);
float HX711_GetWeight(int weightOffset);
int HX711_Tare(int *weightOffset);
