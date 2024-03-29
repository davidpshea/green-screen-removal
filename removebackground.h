#ifndef REMOVEBACKGROUND_H
#define REMOVEBACKGROUND_H

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

QImage* removeBackground(const QImage* inputImage, const QImage* backgroundImage, int threshold, const QRgb& backgroundColour);

#endif
