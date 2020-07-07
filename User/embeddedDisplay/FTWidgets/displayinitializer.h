#ifndef FTDISPLAYINITIALIZER_H
#define FTDISPLAYINITIALIZER_H

#include "ftwidget.h"
#include "riverdieve.h"

class DisplayInitializer: public FTWidget
{
    Q_OBJECT
public:
    explicit DisplayInitializer(Gpu_Hal_Context_t * host);

signals:
    void initialized();

    // FTWidget interface
protected:
    void setup() override;
    void loop() override;
};

#endif // FTDISPLAYINITIALIZER_H
