#ifndef FTDISPLAYINITIALIZER_H
#define FTDISPLAYINITIALIZER_H

#include "ftwidget.h"

class DisplayInitializer: public FTWidget
{
    Q_OBJECT
public:
    explicit DisplayInitializer(FT801_SPI * ft801);

signals:
    void initialized();

    // FTWidget interface
protected:
    void setup() override;
    void loop() override;
private:
    bool isFT801present();
};

#endif // FTDISPLAYINITIALIZER_H
