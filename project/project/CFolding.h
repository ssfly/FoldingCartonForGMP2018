#pragma once

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <qlabel.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <fstream>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <math.h>
#include <qscrollarea.h>
#include <QGraphicsScene>
#include <QTimer>
#include <vector>
#include <string>
#include <iostream>
#include "ui_CFolding.h"
#include "Viewer.h"
#include "FlatMesh.h"
#include "StereoMesh.h"
#include "FAlgorithm.h"

class CFolding : public QMainWindow
{
	Q_OBJECT

public:
	CFolding(QWidget *parent = Q_NULLPTR);

private:
	Ui::CFoldingClass ui;
	Viewer *m_view3d;
	Viewer *m_view2d;

	FlatMesh *m_flatMesh;
	StereoMesh *m_steroMesh;
	FAlgorithm *m_algorithm;

	QHBoxLayout *m_candiArea;

	QString modelPath;
	QString layoutPath;

	vector < vector < MyMesh::VertexHandle >> m_candiVertices;
	vector<Viewer*> viewCandi;
	int canWidth, canHeight;

	qglviewer::Vec bbMax;
	qglviewer::Vec bbMin;

	void focusOnObj(MyMesh &mesh, Viewer *view);

public slots:
	void loadButton();
	void saveButton();
	void initializeButton();
	void optimizeButton();
	void moveButton();
	void vertexButton();
	void edgeButton();
	void faceButton();
	void symmetryButton();
	void chooseCandidate(int idx);
	void mergeVertex();
	void test();
	

};
