#include "MainWindow.h"

#include <QFileDialog>

#include <QPainter>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), volume(0)
{
	ui = new Ui_MainWindow();
	ui->setupUi(this);
	ui->progressBar->hide();
	glWidget = ui->glWidget;

	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeAction()));

	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileAction()));
	connect(this, &MainWindow::dataLoaded, glWidget, &GLWidget::dataLoaded);

	connect(ui->horizontalSlider_0, &QSlider::valueChanged, glWidget, &GLWidget::setBackgroundColor);

}

MainWindow::~MainWindow()
{
	delete volume;
}


//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void MainWindow::openFileAction()
{

	QString filename = QFileDialog::getOpenFileName(this, "Data File", 0, tr("Data Files (*.dat)"));

	if (!filename.isEmpty())
	{
		// store filename
		fileType.filename = filename;
		std::string fn = filename.toStdString();
		bool success = false;

		// progress bar and top label
		ui->progressBar->show();
		ui->progressBar->setEnabled(true);
		ui->labelTop->setText("Loading data ...");

		// load data according to file extension
		if (fn.substr(fn.find_last_of(".") + 1) == "dat")		// LOAD VOLUME
		{
			// create VOLUME
			fileType.type = VOLUME;
			volume = new Volume();

			// load file
			success = volume->loadFromFile(filename, ui->progressBar);
		}

		ui->progressBar->setEnabled(false);
		ui->progressBar->hide();

		// status message
		if (success)
		{
			QString type;
			if (fileType.type == VOLUME) {
				type = "VOLUME";
				emit dataLoaded(volume);
			}
			ui->labelTop->setText("File LOADED [" + filename + "] - Type [" + type + "]");
		}
		else
		{
			ui->labelTop->setText("ERROR loading file " + filename + "!");
			ui->progressBar->setValue(0);
		}
	}
}

void MainWindow::closeAction()
{
	close();
}
