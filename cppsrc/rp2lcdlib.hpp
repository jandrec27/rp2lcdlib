
class LCD{
    public:

    LCD(uint8_t addr, i2c_inst_t* i2c_type,  uint16_t CLK_FREQ, uint8_t SDApin, uint8_t SCLpin);


    LCD();   //Overload function to run LCD with general defaults


    void clear();

    void setCursor(int line, int position);

    void print(const char *s);

    void init();

    private:

};