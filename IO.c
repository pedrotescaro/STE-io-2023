#include<p18f4550.h>
#include <stdio.h>

#define indutivo PORTDbits.RD0
#define capacitivo PORTDbits.RD1
#define sen_grande PORTDbits.RD2
#define sen_media PORTDbits.RD3
#define sen_pequena PORTDbits.RD4
#define rampaG PORTDbits.RD5
#define rampaM PORTDbits.RD6
#define rampaP PORTDbits.RD7
#define motor PORTBbits.RB0
#define valv1 PORTBbits.RB1
#define valv2 PORTBbits.RB2
#define valv3a PORTBbits.RB3
#define valv3r PORTBbits.RB4
#define liga PORTCbits.RC0
#define desliga PORTCbits.RC1

#define lcd_rs PORTAbits.RA5                    
#define lcd_en PORTAbits.RA4                    
#define lcd_data PORTA

int metal=0, grande=0, media=0, pequena=0, plastico=0;
int pcs_metal=0, pcs_plas_grande=0, pcs_plas_media=0, pcs_plas_pequena=0;

void delay(unsigned int ); //GERA??O DE ATRASO       
void LCD_Init(); //INICIALIZA??O DO LCD                   
void LCD_Command(unsigned char ); //ESCRITA DE COMANDO NO LCD  
void LCD_Char(unsigned char x); //ESCRITA DE CARACTERE NO LCD         
void LCD_cursor(char, char); //POSICIONA O CURSOR
void LCD_Clear();//LIMPA O LCD    

LCD_wr_string(char pos_x, char pos_y, char * dado)
{
	char msn[20], qnt_char,x;
	
	LCD_cursor(pos_x,pos_y);
	qnt_char=sprintf(msn,dado);
	for(x=0;x<qnt_char;x++) LCD_Char(msn[x]);
}

LCD_wr_valor(char pos_x, char pos_y, int valor)
{
	char msn[20], qnt_char,x;
	
	LCD_cursor(pos_x,pos_y);
	qnt_char=sprintf(msn,"%i",valor);
	for(x=0;x<qnt_char;x++) LCD_Char(msn[x]);
}



void main(){
	OSCCON=0x72;
	TRISA=0x00;
	PORTA=0x00;
	TRISD=0xFF;
	TRISC=0xFF;
	TRISB=0x00;
	ADCON1=0x0f;
	LCD_Init();                           
	LCD_cursor(3,1);
	LCD_wr_string(1,1,"Pecas Metalicas: ");
	LCD_wr_string(2,1,"Pecas Plasticas G: ");
	LCD_wr_string(3,1,"Pecas Plasticas M: ");
	LCD_wr_string(4,1,"Pecas Plasticas P: ");
	LCD_wr_valor(1,18,pcs_metal);
	LCD_wr_valor(2,19,pcs_plas_grande);
	LCD_wr_valor(3,19,pcs_plas_media);
	LCD_wr_valor(4,19,pcs_plas_pequena);
	

	while(1)
	{
		if(liga && !desliga){
		motor=1;
		}
		if(desliga){
		motor=0;
		metal=0; 
		grande=0; 
		media=0;
		pequena=0;
		plastico=0;
		valv1=0;
		valv2=0;
		valv3a=0;
		valv3r=0;
		}
		if(!indutivo)metal = 1;
		if(!capacitivo && metal == 0)plastico = 1;
		if(!sen_pequena)pequena = 1;
		if(!sen_media)media = 1;
		if(!sen_grande)grande = 1;	

		if(plastico){
			if(grande && media && pequena){
			valv1=1;
			}
			if(!grande && media && pequena){
			valv2=1;
			}
			if(!grande && !media && pequena){
			valv3r=0;
			valv3a=1;
			}
			if(rampaG){
			valv1=0;
			plastico=0;
			pequena=0;
			media=0;
			grande=0;
			pcs_plas_grande++;
			LCD_wr_valor(2,19,pcs_plas_grande);
			}
			if(rampaM){
			valv2=0;
			plastico=0;
			pequena=0;
			media=0;
			grande=0;
			pcs_plas_media++;
			LCD_wr_valor(3,19,pcs_plas_media);
			}
			if(rampaP){
			valv3a = 0;
			valv3r = 1;
			Delay10KTCYx(50);
			plastico=0;
			pequena=0;
			media=0;
			grande=0;
			pcs_plas_pequena++;
			LCD_wr_valor(4,19,pcs_plas_pequena);
			}	
		}
		if(metal && !sen_pequena){
			Delay10KTCYx(100);
			pequena=0;
			media=0;
			grande=0;
			metal=0;
			pcs_metal++;
			LCD_wr_valor(1,18,pcs_metal);
		}
	}
}

void LCD_Init()
{
    delay(200); 
    
    LCD_Command(0x02);   
    LCD_Command(0x2c);              
    LCD_Command(0x01);              
    LCD_Command(0x0c);             
	LCD_Command(0x06);              	   
}

void LCD_Command(unsigned char cmd )
{
	lcd_data = (lcd_data & 0xf0) |(cmd>>4);    
	lcd_rs = 0;                                 
	lcd_en = 1; 
	delay(1);                                 
	lcd_en = 0;
	delay(1);
    lcd_data = (lcd_data & 0xf0) | (cmd & 0x0f);      
	lcd_en = 1;
	delay(1);
	lcd_en = 0;
	delay(5);
}


void LCD_Char(unsigned char dat)
{
	lcd_data = (lcd_data & 0xf0) | (dat>>4);   
	lcd_rs = 1;                                  
	lcd_en = 1; 
	delay(1);                                 
	lcd_en = 0;
	delay(1);
    lcd_data = (lcd_data & 0xf0) | (dat & 0x0f);               
	lcd_en = 1; 
	delay(1);                        
	lcd_en = 0;
	delay(5);
}

void LCD_cursor(char linha,char coluna)
{
    char location=0;
    coluna=coluna-1;
    if(linha==1) location=(0x80 | 0x00)+coluna;    
    if(linha==2) location=(0x80 | 0x40)+coluna;
    if(linha==3) location=(0x80 | 0x14)+coluna;   
    if(linha==4) location=(0x80 | 0x54)+coluna;      
    
    LCD_Command(location);
}
void LCD_Clear()
{
   	LCD_Command(0x01);     
}

void delay(unsigned int val)
{
 unsigned int i,j;
 for(i=0;i<=val;i++)
     for(j=0;j<81;j++);             
 }