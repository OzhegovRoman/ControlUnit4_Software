#ifndef WELCOMEPAGEWIDGET_H
#define WELCOMEPAGEWIDGET_H

#include "ftwidget.h"

class WelcomePageWidget: public FTWidget
{
    Q_OBJECT
public:
    explicit WelcomePageWidget(Gpu_Hal_Context_t * host);

signals:
    void done();
    // FTWidget interface
protected:
    void setup();
};

#endif // WELCOMEPAGEWIDGET_H
