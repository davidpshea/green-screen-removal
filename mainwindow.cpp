#include "mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QSlider>
#include <QFileDialog>
#include <QDebug>
#include <QGroupBox>
#include <QTransform>

#include "debayered.h"
#include "removebackground.h"
#include "imagelabel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(createLayoutWidget());
}

QWidget* MainWindow::createLayoutWidget()
{
    QWidget* rootWidget = new QWidget;

    QHBoxLayout* rootLayout = new QHBoxLayout (rootWidget);
    rootLayout->setAlignment(rootLayout, Qt::AlignLeft | Qt::AlignTop);

    // Controls on left
    QVBoxLayout* sidebar = new QVBoxLayout;
    rootLayout->addLayout(sidebar);
    sidebar->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Group buttons together
    QVBoxLayout* buttonLayout = new QVBoxLayout;
    buttonLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QPushButton* loadButton = new QPushButton("Load Image...");
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadImageButtonClicked()));
    loadButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    buttonLayout->addWidget(loadButton);

    QPushButton* loadBackdropButton = new QPushButton("Load Backdrop...");
    connect(loadBackdropButton, SIGNAL(clicked()), this, SLOT(loadBackgroundImageButtonClicked()));
    loadButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    buttonLayout->addWidget(loadBackdropButton, 0);

    QPushButton* saveImageButton = new QPushButton("Save Image...");
    connect(saveImageButton, SIGNAL(clicked()), this, SLOT(saveImageButtonClicked()));
    loadButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    buttonLayout->addWidget(saveImageButton, 0);

    QPushButton* saveForgroundButton = new QPushButton("Save Forground...");
    connect(saveForgroundButton, SIGNAL(clicked()), this, SLOT(saveForgroundButtonClicked()));
    loadButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    buttonLayout->addWidget(saveForgroundButton, 0);

    QPushButton* rotateButton = new QPushButton("Rotate 90");
    connect(rotateButton, SIGNAL(clicked()), this, SLOT(rotateButtonClicked()));
    rotateButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    buttonLayout->addWidget(rotateButton, 0);

    sidebar->addLayout(buttonLayout);

    QGroupBox* thresholdGroupBox = new QGroupBox(tr("Threshold"));
    QVBoxLayout* thresholdLayout = new QVBoxLayout;

    thresholdGroupBox->setLayout(thresholdLayout);
    sidebar->addWidget(thresholdGroupBox);

    thresholdSlider = new QSlider(Qt::Orientation::Horizontal);
    thresholdSlider->setMinimum(0);
    thresholdSlider->setMaximum(100);
    thresholdSlider->setValue(10);
    thresholdSlider->setTracking(true);
    thresholdSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(thresholdSliderChanged(int)));

    thresholdLayout->addWidget(thresholdSlider);

    QGroupBox* zoomGroupBox = new QGroupBox(tr("Zoom"));
    QVBoxLayout* zoomLayout = new QVBoxLayout;

    QHBoxLayout* imagesLayout = new QHBoxLayout;
    rootLayout->addLayout(imagesLayout);
    imagesLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QImage* blankImage = new QImage(320, 200, QImage::Format_RGB32);
    blankImage->fill(Qt::darkGreen);

    inputImageLabel = new ImageLabel;
    inputImageLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    inputImageLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    inputImageLabel->setImage(blankImage);

    imagesLayout->addWidget(inputImageLabel);

    outputImageLabel = new ImageLabel;
    outputImageLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    outputImageLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    outputImageLabel->setImage(blankImage);
    imagesLayout->addWidget(outputImageLabel);

    return rootWidget;
}

void MainWindow::removeBackgroundFromImage()
{
    outputImage = removeBackground(inputImage, backgroundImage, thresholdSlider->value(), qRgb(32, 32, 32));
}

QImage* MainWindow::loadImage(const QString& title)
{
    QString filename = QFileDialog::getOpenFileName(this,
        title, "./givenimages", tr("Image Files (*.png)")
    );

    if (filename.isNull())
    {
        return nullptr;
    }

    QImage* image = new QImage(filename);
    QImage* debayeredImage = DebayerImageRGGB(image);
    return rotateImage(debayeredImage, imageRotation);
}

void MainWindow::saveImage(const QImage* image, const QString& title)
{
    if ((image != nullptr) && (! image->isNull()))
    {
        QString fileName = QFileDialog::getSaveFileName(this,
            title, ".", tr("Image Files (*.png)"));

        if (! fileName.isNull())
        {
            image->save(fileName);
        }
    }
}

void MainWindow::loadImageButtonClicked()
{
    if (QImage* newImage = loadImage("Select Input Image"))
    {
        inputImage = newImage;
        removeBackgroundFromImage();
        updateUIWithNewImages();
    }
}

void MainWindow::loadBackgroundImageButtonClicked()
{
    if (QImage* newImage = loadImage("Select Background Image"))
    {
        backgroundImage = newImage;
        removeBackgroundFromImage();
        updateUIWithNewImages();
    }
}

QImage* MainWindow::rotateImage(QImage* image, const int angle)
{
    if ((image == nullptr) || image->isNull())
    {
        return nullptr;
    }

    QTransform transform;
    transform.translate(image->width()/2, image->height()/2);
    transform.rotate(static_cast<qreal>(angle)); 
    QImage transformedImage = image->transformed(transform);

    return new QImage (transformedImage);
}

void MainWindow::rotateButtonClicked()
{
    imageRotation = (imageRotation + 90) % 360;

    // rotate all 3 images to keep them in sync
    inputImage = rotateImage(inputImage);
    outputImage = rotateImage(outputImage);
    backgroundImage = rotateImage(backgroundImage);

    updateUIWithNewImages();
}

void MainWindow::saveImageButtonClicked()
{
    saveImage(inputImage, QString("Select Output Image File"));
}

void MainWindow::saveForgroundButtonClicked()
{
    saveImage(outputImage, QString("Select Output Forground File"));
}

void MainWindow::updateUIWithNewImages()
{
    inputImageLabel->setImage(inputImage);
    outputImageLabel->setImage(outputImage);
}

void MainWindow::thresholdSliderChanged(int newValue)
{
    removeBackgroundFromImage();
    updateUIWithNewImages();
}

MainWindow::~MainWindow()
{
}
