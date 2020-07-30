#include <QImage>
#include <QPixmap>
#include <QDebug>

/**
 *  De-Bayer an image - makes assumptions:
 * 1. image is even number of pixels wide and high.
 * 2. image is 32 bit RGB
 */
QImage* DebayerImageRGGB(const QImage* inputImage)
{
    int width = inputImage->width();
    int height = inputImage->height();

    qInfo() << "Image";
    qInfo() << width << height << inputImage->depth();

    QImage* outputImage = new QImage(width / 2, height / 2, QImage::Format_RGB32);

    for (int inputY = 0, outputY = 0; inputY < height; inputY += 2, outputY++)
    {
        const QRgb* scanLines[2] =
        {
           (QRgb*) inputImage->constScanLine(inputY),
           (QRgb*) inputImage->constScanLine(inputY + 1)
        };

        QRgb* outputRow = (QRgb*) outputImage->constScanLine(outputY);

        for (int inputX = 0, outputX = 0; inputX < width; inputX += 2, outputX++)
        {
            QRgb pixels[4] = {
                (scanLines[0])[inputX],
                (scanLines[0])[inputX + 1],
                (scanLines[1])[inputX],
                (scanLines[1])[inputX + 1]
            };

            int red   = qRed(pixels[0]);
            int blue  = qBlue(pixels[3]);
            int green = (qGreen(pixels[1]) + qGreen(pixels[2])) / 2;

            outputRow[outputX] = qRgb(red, green, blue);

            outputRow[outputX] = qRgb(
                qRed(pixels[0]),
                (qGreen(pixels[1]) + qGreen(pixels[2])) / 2,
                qBlue(pixels[3])
            );
        }
    }

    return outputImage;
}


QImage::Format pixmapFormat()
{
    static auto format = QPixmap{1,1}.toImage().format();
    return format;
}