#ifndef LCD_H_
#define LCD_H_

int init_lcd(int ldc_addr);

void close_lcd();

void update_lcd(float ti, float te, float tr);

#endif /* LCD_H_ */