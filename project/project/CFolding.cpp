#include "CFolding.h"
#include <qdebug.h>

CFolding::CFolding(QWidget *parent)
	: QMainWindow(parent), m_steroMesh(NULL)
{
	ui.setupUi(this);

	m_flatMesh = new FlatMesh();
	m_algorithm = new FAlgorithm();
	//m_steroMesh = new StereoMesh();

	m_view2d = new Viewer();
	m_view3d = new Viewer();
	ui.show2dLayout->addWidget(m_view2d);
	ui.show3dLayout->addWidget(m_view3d);

	m_candiArea = new QHBoxLayout();
	ui.scrollAreaWidgetContents->setLayout(m_candiArea);
	canWidth = ui.scrollAreaWidgetContents->height();

	connect(ui.actionLoad_2, SIGNAL(triggered()), this, SLOT(loadButton()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveButton()));
	connect(ui.actionInitialize, SIGNAL(triggered()), this, SLOT(initializeButton()));
	connect(ui.actionOptimize, SIGNAL(triggered()), this, SLOT(optimizeButton()));
	connect(ui.MoveTool, SIGNAL(clicked()), this, SLOT(moveButton()));
	connect(ui.VertexTool, SIGNAL(clicked()), this, SLOT(vertexButton()));
	connect(ui.EdgeTool, SIGNAL(clicked()), this, SLOT(edgeButton()));
	connect(ui.faceTool, SIGNAL(clicked()), this, SLOT(faceButton()));
	connect(ui.SymmetryTool, SIGNAL(clicked()), this, SLOT(symmetryButton()));
	connect(ui.mergeTool, SIGNAL(clicked()), this, SLOT(mergeVertex()));

	connect(ui.actionLoadtest, SIGNAL(triggered()), this, SLOT(test()));
	//connect(ui, SIGNAL(triggered()), this, SLOT());

	
}

void CFolding::test()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle("Open 3D Model");
	fileDialog->setDirectory(".");
	fileDialog->setFilter(QDir::Files | QDir::NoSymLinks);
	if (fileDialog->exec() == QDialog::Accepted)
	{
		modelPath = fileDialog->selectedFiles()[0];
		QMessageBox::information(NULL, tr("Path"), tr("You selected ") + modelPath);
	}
	else
	{
		QMessageBox::information(NULL, tr("Path"), tr("You didn't select any model."));
	}
	m_view3d->mesh.request_vertex_normals();
	m_view3d->mesh.request_face_colors();
	if (!m_view3d->mesh.has_vertex_normals())
	{
		QMessageBox::information(NULL, tr("ERROR"), tr("ERROR: error mesh."));
	}
	OpenMesh::IO::Options opt;
	string pathtmp = modelPath.toStdString();
	if (!OpenMesh::IO::read_mesh(m_view3d->mesh, pathtmp, opt))
	{
		QMessageBox::information(NULL, tr("Path"), tr("Error: Cannot read mesh from") + modelPath);
	}
	if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	{
		m_view3d->mesh.request_face_normals();
		m_view3d->mesh.update_normals();
		m_view3d->mesh.release_face_normals();
	}
	for (MyMesh::FaceIter f_it = m_view3d->mesh.faces_begin(); f_it != m_view3d->mesh.faces_end(); f_it++)
	{
		float r = rand() % 255;
		float g = rand() % 255;
		float b = rand() % 255;
		m_view3d->mesh.set_color(*f_it, MyMesh::Color(r, g, b));
	}
	//showmesh = view3d->mesh;
	double xmin = 100000000.;
	double xmax = -100000000.;
	double ymin = 100000000.;
	double ymax = -100000000.;
	double zmin = 100000000.;
	double zmax = -100000000.;

	for (MyMesh::VertexIter v_it = m_view3d->mesh.vertices_begin(); v_it != m_view3d->mesh.vertices_end(); ++v_it)
	{
		xmin = m_view3d->mesh.point(*v_it).data()[0] < xmin ? m_view3d->mesh.point(*v_it).data()[0] : xmin;
		ymin = m_view3d->mesh.point(*v_it).data()[1] < ymin ? m_view3d->mesh.point(*v_it).data()[1] : ymin;
		zmin = m_view3d->mesh.point(*v_it).data()[2] < zmin ? m_view3d->mesh.point(*v_it).data()[2] : zmin;
		xmax = m_view3d->mesh.point(*v_it).data()[0] > xmax ? m_view3d->mesh.point(*v_it).data()[0] : xmax;
		ymax = m_view3d->mesh.point(*v_it).data()[1] > ymax ? m_view3d->mesh.point(*v_it).data()[1] : ymax;
		zmax = m_view3d->mesh.point(*v_it).data()[2] > zmax ? m_view3d->mesh.point(*v_it).data()[2] : zmax;
	}
	//bbMax = qglviewer::Vec(xmax, ymax, zmax);
	//bbMin = qglviewer::Vec(xmin, ymin, zmin);
	m_view3d->getEdgeVertex();
	m_view3d->camera()->setRevolveAroundPoint(qglviewer::Vec(.5*(xmin + xmax), .5*(ymin + ymax), .5*(zmin + zmax)));
	m_view3d->camera()->setSceneBoundingBox(qglviewer::Vec(xmin, ymin, zmin), qglviewer::Vec(xmax, ymax, zmax));
	m_view3d->camera()->showEntireScene();
	m_view3d->update();
}

void CFolding::focusOnObj(MyMesh &mesh, Viewer *view)
{
	double xmin = 100000000.;
	double xmax = -100000000.;
	double ymin = 100000000.;
	double ymax = -100000000.;
	double zmin = 100000000.;
	double zmax = -100000000.;
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		xmin = mesh.point(*v_it).data()[0] < xmin ? mesh.point(*v_it).data()[0] : xmin;
		ymin = mesh.point(*v_it).data()[1] < ymin ? mesh.point(*v_it).data()[1] : ymin;
		zmin = mesh.point(*v_it).data()[2] < zmin ? mesh.point(*v_it).data()[2] : zmin;
		xmax = mesh.point(*v_it).data()[0] > xmax ? mesh.point(*v_it).data()[0] : xmax;
		ymax = mesh.point(*v_it).data()[1] > ymax ? mesh.point(*v_it).data()[1] : ymax;
		zmax = mesh.point(*v_it).data()[2] > zmax ? mesh.point(*v_it).data()[2] : zmax;
	}
	bbMax = qglviewer::Vec(xmax, ymax, zmax);
	bbMin = qglviewer::Vec(xmin, ymin, zmin);
	view->camera()->setRevolveAroundPoint(qglviewer::Vec(.5 * (bbMin[0] + bbMax[0]), .5 * (bbMin[1]+ bbMax[1]), .5 * (bbMin[2] + bbMax[2])));
	view->camera()->setSceneBoundingBox(bbMin, bbMax);
	view->camera()->showEntireScene();
}
void CFolding::loadButton()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle("Open 2D layout");
	fileDialog->setDirectory(".");
	fileDialog->setFilter(QDir::Files | QDir::NoSymLinks);
	if (fileDialog->exec() == QDialog::Accepted)
	{
		layoutPath = fileDialog->selectedFiles()[0];
		QMessageBox::information(NULL, tr("Path"), tr("You selected ") + layoutPath);
	}
	else
	{
		QMessageBox::information(NULL, tr("Path"), tr("You didn't select any layout. "));
		return;
	}
	if (m_view2d->isUsed)
	{
		m_view2d->clearViewer();
		m_view3d->clearViewer();
		m_flatMesh = new FlatMesh();
		m_algorithm = new FAlgorithm();
		for (int i = 0; i < viewCandi.size(); i++)
		{
			m_candiArea->removeWidget(viewCandi[i]);
			delete viewCandi[i];
		}
		viewCandi.clear();
		m_candiVertices.clear();
		if (m_steroMesh != nullptr) delete m_steroMesh;
	}
	m_flatMesh->createMesh(layoutPath.toStdString(), m_view2d->mesh);
	m_view2d->isUsed = true;
	//////////////////////////////////////////
	//flatMesh->readLineListFromTxt(layoutPath.toStdString);
	//Get the 2d mesh;
	//m_view2d->mesh = ...... ////send the mesh to the class view2d;
	focusOnObj(m_view2d->mesh, m_view2d);
	m_view2d->needUpdateColor = false;
	m_view2d->getEdgeVertex();
	m_view2d->update();
	//////////////////////////////////////////
}

void CFolding::saveButton()
{
}

void CFolding::initializeButton()
{
	m_steroMesh = new StereoMesh(m_view2d->mesh);
	m_view3d->mesh = m_steroMesh->folding();
	m_view3d->getEdgeVertex();
	focusOnObj(m_view3d->mesh, m_view3d);
	m_view3d->update();

	m_view2d->setColorIdx(m_view3d->getColorIdx());
	qDebug() << m_view3d->getColorIdx();
 	m_candiVertices = m_algorithm->autoDetection(m_view3d->mesh);
	int n = m_candiVertices.size();
	vector<MyMesh::VertexHandle> candiTmp;
	ui.scrollAreaWidgetContents->setFixedSize( n * canWidth, canWidth);
	viewCandi.resize(n);

	for (int i = 0; i < n; i++)
	{
		viewCandi[i] = new Viewer();
		viewCandi[i]->viewIdx = i;
		viewCandi[i]->resize(canWidth, canWidth);
		viewCandi[i]->mesh = m_view3d->mesh;
		viewCandi[i]->isCandidate = true;
		m_candiArea->addWidget(viewCandi[i]);
		for (int j = 0; j < m_candiVertices[i].size(); j++)
		{
			viewCandi[i]->mergeVertices.push_back(m_candiVertices[i][j].idx());
		}
		focusOnObj(viewCandi[i]->mesh, viewCandi[i]);
		viewCandi[i]->getEdgeVertex();
		connect(viewCandi[i], SIGNAL(isDoubleClicked(int)), this, SLOT(chooseCandidate(int)));
		viewCandi[i]->update();
	}

	//initialize solver, add basic constrains
	m_algorithm->InitializeShapeConstrain(m_view3d->mesh);

	///////////////////////////////////////////////
	//get the initial model
	//m_view3d->mesh = .......
	//m_view3d->update;
	//get the mergeVerticesList
	//vector<Viewer*> viewCandi(n);
	///////////////////////////////////////////////
}
void CFolding::chooseCandidate(int idx)
{
	vector<MyMesh::VertexHandle> merge_v;
	for (int i = 0; i < viewCandi[idx]->mergeVertices.size(); i++)
	{
		MyMesh::VertexHandle htmp(viewCandi[idx]->mergeVertices[i]);
		merge_v.push_back(htmp);
	}

	// optimize on chosen candidate
	m_view3d->mesh = m_algorithm->ShapeOptimization(m_view3d->mesh, merge_v);
	m_view3d->getEdgeVertex();
	focusOnObj(m_view3d->mesh, m_view3d);
	m_view3d->update();


}
void CFolding::mergeVertex()
{

}
void CFolding::optimizeButton()
{
	////////////////////////////////////////////
	//optimize()
	//m_view3d->mesh = ...
	//m_view3d->update();
	////////////////////////////////////////////


	if (m_view3d->selected_vertices.size() > 0)
	{
		vector<MyMesh::VertexHandle> vhs;
		for (int i = 0; i < m_view3d->selected_vertices.size(); i++)
		{
			MyMesh::VertexHandle vh((m_view3d->selected_vertices).at(i));
			vhs.push_back(vh);
		}
		m_view3d->mesh = m_algorithm->ShapeOptimization(m_view3d->mesh, vhs);
	}

	if (m_view3d->selected_faces.size() > 0)
	{
		vector<MyMesh::FaceHandle> fhs;
		for (int i = 0; i < m_view3d->selected_faces.size(); i++)
		{
			MyMesh::FaceHandle fh((m_view3d->selected_faces).at(i));
			fhs.push_back(fh);
		}
		m_view3d->mesh = m_algorithm->ShapeOptimization(m_view3d->mesh, fhs);
	}
	

	m_view3d->getEdgeVertex();
	focusOnObj(m_view3d->mesh, m_view3d);
	m_view3d->update();

}

void CFolding::moveButton()
{
	m_view3d->type_ = Viewer::Type::NONE_;
	m_view3d->selected_edges.clear();
	m_view3d->selected_faces.clear();
	m_view3d->selected_vertices.clear();
}

void CFolding::vertexButton()
{
	//set the choose mode as VERTEX
	m_view3d->type_ = Viewer::Type::VERTEX;  
	m_view3d->selected_edges.clear();
	m_view3d->selected_faces.clear();
	m_view3d->selected_vertices.clear();
}

void CFolding::edgeButton()
{
	//set the choose mode as EDGE
	m_view3d->type_ = Viewer::Type::EDGE;
	m_view3d->selected_edges.clear();
	m_view3d->selected_faces.clear();
	m_view3d->selected_vertices.clear();
}

void CFolding::faceButton()
{
	m_view3d->type_ = Viewer::Type::FACE;
	m_view3d->selected_edges.clear();
	m_view3d->selected_faces.clear();
	m_view3d->selected_vertices.clear();
}
void CFolding::symmetryButton()
{
	//m_view3d->selected_vertices
	//test for symmetry vertex pair
	
	for (int i = 0; i < viewCandi.size(); i++)
		m_candiArea->removeWidget(viewCandi[i]);

	/*MyMesh::VertexHandle vh1((m_view3d->selected_vertices).at(0));
	MyMesh::VertexHandle vh2((m_view3d->selected_vertices).at(1));

	m_candiVertices = m_algorithm->symmetricVertexPair(m_view3d->mesh, vh1 , vh2);*/

	vector<MyMesh::VertexHandle> vhs;
	for (int i = 0; i < m_view3d->selected_vertices.size(); i++)
	{
		MyMesh::VertexHandle vh((m_view3d->selected_vertices).at(i));
		vhs.push_back(vh);
	}
	m_candiVertices = m_algorithm->symmetryDetection(m_view3d->mesh, vhs);

	int n = m_candiVertices.size();
	vector<MyMesh::VertexHandle> candiTmp;
	ui.scrollAreaWidgetContents->setFixedSize(n * canWidth, canWidth);
	viewCandi.resize(n);

	for (int i = 0; i < n; i++)
	{
		viewCandi[i] = new Viewer();
		viewCandi[i]->resize(canWidth, canWidth);
		viewCandi[i]->mesh = m_view3d->mesh;
		viewCandi[i]->isCandidate = true;
		m_candiArea->addWidget(viewCandi[i]);
		for (int j = 0; j < m_candiVertices[i].size(); j++)
		{
			viewCandi[i]->mergeVertices.push_back(m_candiVertices[i][j].idx());
		}
		viewCandi[i]->getEdgeVertex();
		focusOnObj(viewCandi[i]->mesh, viewCandi[i]);
		viewCandi[i]->update();
	}
}
