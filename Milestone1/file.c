int main(){
	
   interrupt(0x21,0x0, "Hello World!", 0, 0);
   while(1){}	
   return 0;
}