#include "cPCH.h"
#include "cDisplay.h"
#include <sstream>

#define CANVAS_WIDTH GetSize().GetWidth()
#define CANVAS_HEIGHT GetSize().GetHeight()

double clamp(double val, double lo, double hi) {
	if (val < lo){
		return lo;
	}
	if (val > hi){
		return hi;
	}
	return val;
}

cDisplay::cDisplay(wxWindow* parent) : wxGLCanvas(parent), m_Context(this)
{
	Bind(wxEVT_PAINT, &cDisplay::OnPaint, this);
	SetCurrent(m_Context);
	InitializeGLEW();
	Setup();

	//SetBackgroundColour({ 255,255,255,255 });
	//SetBackgroundStyle(wxBG_STYLE_PAINT);

	Bind(wxEVT_LEFT_DOWN, &cDisplay::OnLeftMouseToggle, this);
	Bind(wxEVT_LEFT_UP, &cDisplay::OnLeftMouseToggle, this);
	Bind(wxEVT_MOTION, &cDisplay::OnMouseMove, this);
	Bind(wxEVT_MOUSEWHEEL, &cDisplay::OnMouseWheel, this);
	Bind(wxEVT_SIZE, &cDisplay::OnWindowResize, this);
}

cDisplay::~cDisplay()
{
	SetCurrent(m_Context);
	delete[] m_LineData;
	delete[] m_TextureData;
}

void cDisplay::InitializeGLEW()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		const GLubyte* msg = glewGetErrorString(err);
		throw std::exception(reinterpret_cast<const char*>(msg));
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void cDisplay::OnPaint(wxPaintEvent& event)
{
	SetCurrent(m_Context);

	// set background to black
	glClearColor(pallete_background.r, pallete_background.g, pallete_background.b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw the graphics
	if(show_grid){
		DrawGridLines();
		DrawGridCoords();
	}

	DrawGraph();

	if (m_tool == Tool::Select && keyboardstatememory == wxMOD_SHIFT)
		DrawSelectionBox();

	// and display
	glFlush();
	SwapBuffers();
}

void cDisplay::Setup()
{
	// upload vertex data
	m_Line_vbo = std::make_shared<cOpenGLDynamicVBO>(2 * maxVertices * sizeof(LineVertex));
	m_Texture_vbo = std::make_shared<cOpenGLDynamicVBO>(6 * maxVertices * sizeof(TextureVertex));

	m_Line_shader = std::make_shared<cOpenGLShader>("src/assets/shaders/Line_Shader.glsl");
	m_Texture_shader = std::make_shared<cOpenGLShader>("src/assets/shaders/Texture_Shader.glsl");

	// setup vertex array objects
	m_Line_vao = std::make_shared<cOpenGLVAO>();
	m_Line_vao->BindVertexBuffer(m_Line_vbo);
	m_Line_vao->SetLayout({ {2,GL_FLOAT}, {3,GL_FLOAT} });

	m_Texture_vao = std::make_shared<cOpenGLVAO>();
	m_Texture_vao->BindVertexBuffer(m_Texture_vbo);
	m_Texture_vao->SetLayout({ {2,GL_FLOAT}, {3,GL_FLOAT}, {2,GL_FLOAT} });

	uint32_t* m_Texture_indices = new uint32_t[6 * maxVertices];
	uint8_t ib_div[6] = { 0,1,2,1,2,3 };
	for (uint32_t i = 0; i < maxVertices; i++) {
		for (uint8_t j = 0; j < 6; j++) {
			m_Texture_indices[6 * i + j] = 4 * i + ib_div[j];
		}
	}
	m_Texture_ibo = std::make_shared<cOpenGLStaticIBO>(6 * maxVertices * sizeof(uint32_t), m_Texture_indices, GL_UNSIGNED_INT);
	delete[] m_Texture_indices;

	m_VertexTexture = std::make_shared<cOpenGLTexture>("src/assets/vertex.png");
	m_FontTexture = std::make_shared<cOpenGLTexture>("src/assets/font_texture.png");
}

void cDisplay::SetBGColor(const wxColor& color)
{
	pallete_background = wxtoglmColor(color);
	pallete_subgrid = (glmcolor(3) * wxtoglmColor(color) + pallete_axes) / glmcolor(4);
	pallete_grid = (wxtoglmColor(color) + pallete_axes) / glmcolor(2);
}
void cDisplay::SetGridColor(const wxColor& color)
{
	pallete_subgrid = (wxtoglmColor(color) + glmcolor(3) * pallete_background) / glmcolor(4);
	pallete_grid = (wxtoglmColor(color) + pallete_background) / glmcolor(2);
	pallete_axes = wxtoglmColor(color);
}
void cDisplay::SetVertexColor(const wxColor& color)
{
	pallete_vertex = wxtoglmColor(color);
}
void cDisplay::SetSelectedVertexColor(const wxColor& color)
{
	pallete_selectedvertex = wxtoglmColor(color);
}
wxColor cDisplay::GetBGColor()
{
	return glmtowxColor(pallete_background);
}
wxColor cDisplay::GetGridColor()
{
	return glmtowxColor(pallete_axes);
}
wxColor cDisplay::GetVertexColor()
{
	return glmtowxColor(pallete_vertex);
}
wxColor cDisplay::GetSelectedVertexColor()
{
	return glmtowxColor(pallete_selectedvertex);
}

void cDisplay::SelectVertex(uint32_t vindex)
{
	if (!std::binary_search(selected_vertices.begin(), selected_vertices.end(), vindex)) {
		auto iter = std::find_if(selected_vertices.begin(), selected_vertices.end(), [&](uint32_t v) { return v > vindex; });
		selected_vertices.insert(iter, vindex);
	}
}
void cDisplay::DeselectVertex(uint32_t vindex)
{
	if (std::binary_search(selected_vertices.begin(), selected_vertices.end(), vindex))
		selected_vertices.erase(std::find(selected_vertices.begin(), selected_vertices.end(), vindex));
}
void cDisplay::ToggleSelectVertex(uint32_t vindex)
{
	if (std::binary_search(selected_vertices.begin(), selected_vertices.end(), vindex))
		selected_vertices.erase(std::find(selected_vertices.begin(), selected_vertices.end(), vindex));
	else {
		auto iter = std::find_if(selected_vertices.begin(), selected_vertices.end(), [&](uint32_t v) { return v > vindex; });
		selected_vertices.insert(iter, vindex);
	}
}
bool cDisplay::IsVertexSelected(uint32_t vindex)
{
	return std::binary_search(selected_vertices.begin(), selected_vertices.end(), vindex);
}
void cDisplay::SelectAll() {
	selected_vertices.clear();
	selected_vertices.reserve(m_Graph.size());
	for (uint32_t i = 0; i < m_Graph.size(); i++) {
		selected_vertices.push_back(i);
	}
}
void cDisplay::DeselectAll() {
	selected_vertices.clear();
}
void cDisplay::ToggleSelection() {
	std::vector<uint32_t> old_selection = selected_vertices;
	selected_vertices.clear();
	selected_vertices.reserve(m_Graph.size() - old_selection.size());
	uint32_t intstart = 0;
	for (uint32_t& vindex : old_selection) {
		for (int64_t i = intstart; i < vindex; i++) {
			selected_vertices.push_back(i);
		}
		intstart = vindex + 1;
	}
	for (int64_t i = intstart; i < m_Graph.size(); i++) {
		selected_vertices.push_back(i);
	}

}
void cDisplay::DeleteSelection() {
	for (int64_t iter = selected_vertices.size() - 1; iter >= 0; iter--) {
		m_Graph.RemoveVertex(selected_vertices[iter]);
	}
	selected_vertices.clear();
}
void cDisplay::SetSelectionMode(SelectMode mode) {
	m_selectmode = mode;
}

glm::dvec2 cDisplay::ViewtoWorld(const glm::vec2& viewv) {
	glm::dvec2 worldv;
	worldv.x = (viewv.x - CANVAS_WIDTH / 2) * display_scale + display_pos.x;
	worldv.y = (CANVAS_HEIGHT / 2 - viewv.y) * display_scale + display_pos.y;
	return worldv;
}
glm::dvec2 cDisplay::ViewtoWorld(const wxPoint& viewv) {
	glm::dvec2 worldv;
	worldv.x = (viewv.x - CANVAS_WIDTH / 2) * display_scale + display_pos.x;
	worldv.y = (CANVAS_HEIGHT / 2 - viewv.y) * display_scale + display_pos.y;
	return worldv;
}
glm::vec2 cDisplay::glmWorldtoView(const glm::dvec2& worldv) {
	glm::vec2 viewv;
	viewv.x = (worldv.x - display_pos.x) / display_scale + CANVAS_WIDTH / 2;
	viewv.y = CANVAS_HEIGHT / 2 - (worldv.y - display_pos.y) / display_scale;
	return viewv;
}
wxPoint cDisplay::wxWorldtoView(const glm::dvec2& worldv) {
	wxPoint viewv;
	viewv.x = (worldv.x - display_pos.x) / display_scale + CANVAS_WIDTH / 2;
	viewv.y = CANVAS_HEIGHT / 2 - (worldv.y - display_pos.y) / display_scale;
	return viewv;
}

void cDisplay::DrawGridLines()
{
	double scale = log10(std::max(CANVAS_WIDTH, CANVAS_HEIGHT) * display_scale) + 0.2;
	double gridsize = pow(10, floor(scale));
	bool subgrid = false;
	if (gridsize / display_scale > 0.5*std::min(CANVAS_WIDTH, CANVAS_HEIGHT)) {
		subgrid = true;
		gridsize = pow(10, floor(scale - 1));
	}

	glm::dvec2 v0 = ViewtoWorld(wxPoint(0,CANVAS_HEIGHT));
	glm::dvec2 v1 = ViewtoWorld(wxPoint(CANVAS_WIDTH,0));

	int index = 0;
	if (subgrid) {
		for (int i = ceil(v0.x / gridsize); i < v1.x / gridsize; i++) {
			if (i%10) {
				m_LineData[index].position = { (i * gridsize - display_pos.x) / display_scale + CANVAS_WIDTH / 2, 0 };
				m_LineData[index+1].position = {(i * gridsize - display_pos.x) / display_scale + CANVAS_WIDTH / 2, CANVAS_HEIGHT};
				m_LineData[index].color = pallete_subgrid;
				m_LineData[index + 1].color = pallete_subgrid;
				index+=2;
			}
		}
		for (int j = ceil(v0.y / gridsize); j < v1.y / gridsize; j++) {
			if (j % 10) {
				m_LineData[index].position = { 0, CANVAS_HEIGHT / 2 - (j * gridsize - display_pos.y) / display_scale };
				m_LineData[index+1].position = { CANVAS_WIDTH, CANVAS_HEIGHT / 2 - (j * gridsize - display_pos.y) / display_scale };
				m_LineData[index].color = pallete_subgrid;
				m_LineData[index + 1].color = pallete_subgrid;
				index += 2;
			}
		}
		for (int i = ceil(v0.x / (10 * gridsize)); i < v1.x / (10 * gridsize); i++) {
			if (i) {
				m_LineData[index].position = { (i * (10 * gridsize) - display_pos.x) / display_scale + CANVAS_WIDTH / 2, 0 };
				m_LineData[index+1].position = {(i * (10 * gridsize) - display_pos.x) / display_scale + CANVAS_WIDTH / 2, CANVAS_HEIGHT};
				m_LineData[index].color = pallete_grid;
				m_LineData[index + 1].color = pallete_grid;
				index+=2;
			}
		}
		for (int j = ceil(v0.y / (10 * gridsize)); j < v1.y / (10 * gridsize); j++) {
			if (j) {
				m_LineData[index].position = { 0, CANVAS_HEIGHT / 2 - (j * (10 * gridsize) - display_pos.y) / display_scale };
				m_LineData[index+1].position = { CANVAS_WIDTH, CANVAS_HEIGHT / 2 - (j * (10 * gridsize) - display_pos.y) / display_scale };
				m_LineData[index].color = pallete_grid;
				m_LineData[index + 1].color = pallete_grid;
				index += 2;
			}
		}
		m_LineData[index].position = { ( - display_pos.x) / display_scale + CANVAS_WIDTH / 2, 0 };
		m_LineData[index + 1].position = { ( - display_pos.x) / display_scale + CANVAS_WIDTH / 2, CANVAS_HEIGHT };
		m_LineData[index].color = pallete_axes;
		m_LineData[index + 1].color = pallete_axes;
		index += 2;
		m_LineData[index].position = { 0, CANVAS_HEIGHT / 2 - ( - display_pos.y) / display_scale };
		m_LineData[index + 1].position = { CANVAS_WIDTH, CANVAS_HEIGHT / 2 - ( - display_pos.y) / display_scale };
		m_LineData[index].color = pallete_axes;
		m_LineData[index + 1].color = pallete_axes;
		index += 2;
	}
	else {
		for (int i = ceil(v0.x / gridsize); i < v1.x / gridsize; i++) {
			if (i) {
				m_LineData[index].position = { (i * gridsize - display_pos.x) / display_scale + CANVAS_WIDTH / 2, 0 };
				m_LineData[index + 1].position = { (i * gridsize - display_pos.x) / display_scale + CANVAS_WIDTH / 2, CANVAS_HEIGHT };
				m_LineData[index].color = pallete_subgrid;
				m_LineData[index + 1].color = pallete_subgrid;
				index += 2;
			}
		}
		for (int j = ceil(v0.y / gridsize); j < v1.y / gridsize; j++) {
			if (j) {
				m_LineData[index].position = { 0, CANVAS_HEIGHT / 2 - (j * gridsize - display_pos.y) / display_scale };
				m_LineData[index + 1].position = { CANVAS_WIDTH, CANVAS_HEIGHT / 2 - (j * gridsize - display_pos.y) / display_scale };
				m_LineData[index].color = pallete_subgrid;
				m_LineData[index + 1].color = pallete_subgrid;
				index += 2;
			}
		}
		
		m_LineData[index].position = { (-display_pos.x) / display_scale + CANVAS_WIDTH / 2, 0 };
		m_LineData[index + 1].position = { (-display_pos.x) / display_scale + CANVAS_WIDTH / 2, CANVAS_HEIGHT };
		m_LineData[index].color = pallete_axes;
		m_LineData[index + 1].color = pallete_axes;
		index += 2;
		m_LineData[index].position = { 0, CANVAS_HEIGHT / 2 - (-display_pos.y) / display_scale };
		m_LineData[index + 1].position = { CANVAS_WIDTH, CANVAS_HEIGHT / 2 - (-display_pos.y) / display_scale };
		m_LineData[index].color = pallete_axes;
		m_LineData[index + 1].color = pallete_axes;
		index += 2;
	}
	m_Line_vbo->InsertData(index * sizeof(LineVertex), m_LineData);
	m_Line_vao->BindVertexBuffer(m_Line_vbo);
	m_Line_shader->Use();
	m_Line_vao->DrawArrays(GL_LINES);
}

void cDisplay::DrawGridCoords() {
	double scale = log10(std::max(CANVAS_WIDTH, CANVAS_HEIGHT) * display_scale) + 0.2;
	double gridsize = pow(10, floor(scale));

	glm::dvec2 v0 = ViewtoWorld(wxPoint(0, CANVAS_HEIGHT));
	glm::dvec2 v1 = ViewtoWorld(wxPoint(CANVAS_WIDTH, 0));

	uint32_t index = 0;
	std::stringstream coordstr;

	for (int i = floor(v0.x / gridsize); i < v1.x / gridsize; i++) {
		for (int j = floor(v0.y / gridsize); j < v1.y / gridsize; j++) {
			coordstr.str(std::string());
			coordstr << "(" << i * gridsize << "," << j * gridsize << ")";
			coordstr.seekp(0, std::ios::end);
			int kmax = coordstr.tellp();
			coordstr.seekp(0, std::ios::beg);
			for (int k = 0; k < kmax; k++) {
				m_TextureData[4 * index].position = glmWorldtoView({ i * gridsize,j * gridsize }) + glm::vec2(8.0f * k, -12.0f);
				m_TextureData[4 * index + 1].position = glmWorldtoView({ i * gridsize,j * gridsize }) + glm::vec2(8.0f * k + 8.0f, -12.0f);
				m_TextureData[4 * index + 2].position = glmWorldtoView({ i * gridsize,j * gridsize }) + glm::vec2(8.0f * k, 0.0f);
				m_TextureData[4 * index + 3].position = glmWorldtoView({ i * gridsize,j * gridsize }) + glm::vec2(8.0f * k + 8.0f, 0.0f);

				m_TextureData[4 * index].color = pallete_axes;
				m_TextureData[4 * index + 1].color = pallete_axes;
				m_TextureData[4 * index + 2].color = pallete_axes;
				m_TextureData[4 * index + 3].color = pallete_axes;

				char ch; coordstr.read(&ch,sizeof(char));
				m_TextureData[4 * index].texcoord = glm::vec2(8 * (ch % 16), 12 * (floor(ch / 16) - 2)) + glm::vec2(0, 0);
				m_TextureData[4 * index + 1].texcoord = glm::vec2(8*(ch % 16), 12 * (floor(ch / 16) - 2)) + glm::vec2(8, 0);
				m_TextureData[4 * index + 2].texcoord = glm::vec2(8*(ch % 16), 12 * (floor(ch / 16) - 2)) + glm::vec2(0, 12);
				m_TextureData[4 * index + 3].texcoord = glm::vec2(8*(ch % 16), 12 * (floor(ch / 16) - 2)) + glm::vec2(8, 12);

				index++;
			}
		}
	}

	m_Texture_vbo->InsertData(4 * index * sizeof(TextureVertex), m_TextureData);
	m_Texture_vao->BindVertexBuffer(m_Texture_vbo);
	m_Texture_shader->Use();
	m_Texture_shader->SetUniformVec2("u_texSize", 128, 72);
	m_Texture_shader->SetUniformSampler2D("u_texture", 0);
	m_FontTexture->Bind();
	m_Texture_vao->DrawElements(m_Texture_ibo, GL_TRIANGLES, 6 * index);
}

void cDisplay::DrawGraph()
{
	int drawcalls = m_Graph.size() / maxVertices;
	int remainder = m_Graph.size() - drawcalls * maxVertices;
	size_t index = 0;
	for (int i = 0; i < drawcalls; i++) {
		index = 0;
		for (int j = 0; j < maxVertices; j++) {
			m_TextureData[index].position = glmWorldtoView(m_Graph[i * maxVertices + j]) + glm::vec2(-3.0f, -3.0f);
			m_TextureData[index + 1].position = glmWorldtoView(m_Graph[i * maxVertices + j]) + glm::vec2(3.0f, -3.0f);
			m_TextureData[index + 2].position = glmWorldtoView(m_Graph[i * maxVertices + j]) + glm::vec2(-3.0f, 3.0f);
			m_TextureData[index + 3].position = glmWorldtoView(m_Graph[i * maxVertices + j]) + glm::vec2(3.0f, 3.0f);

			if (IsVertexSelected(i * maxVertices + j)) {
				m_TextureData[index].color = pallete_selectedvertex;
				m_TextureData[index + 1].color = pallete_selectedvertex;
				m_TextureData[index + 2].color = pallete_selectedvertex;
				m_TextureData[index + 3].color = pallete_selectedvertex;
			}
			else {
				m_TextureData[index].color = pallete_vertex;
				m_TextureData[index + 1].color = pallete_vertex;
				m_TextureData[index + 2].color = pallete_vertex;
				m_TextureData[index + 3].color = pallete_vertex;
			}

			m_TextureData[index].texcoord = { 0,0 };
			m_TextureData[index + 1].texcoord = { 16,0 };
			m_TextureData[index + 2].texcoord = { 0,16 };
			m_TextureData[index + 3].texcoord = { 16,16 };

			index += 4;
		}
		m_Texture_vbo->InsertData(maxVertices * 4 * sizeof(TextureVertex), m_TextureData);
		m_Texture_vao->BindVertexBuffer(m_Texture_vbo);
		m_Texture_shader->Use();
		m_Texture_shader->SetUniformVec2("u_texSize", 16, 16);
		m_Texture_shader->SetUniformSampler2D("u_texture", 0);
		m_VertexTexture->Bind();
		m_Texture_vao->DrawElements(m_Texture_ibo, GL_TRIANGLES, 6 * maxVertices);
	}

	index = 0;
	for (int j = 0; j < remainder; j++) {
		m_TextureData[index].position = glmWorldtoView(m_Graph[drawcalls * maxVertices + j]) + glm::vec2(-3.0f, -3.0f);
		m_TextureData[index + 1].position = glmWorldtoView(m_Graph[drawcalls * maxVertices + j]) + glm::vec2(3.0f, -3.0f);
		m_TextureData[index + 2].position = glmWorldtoView(m_Graph[drawcalls * maxVertices + j]) + glm::vec2(-3.0f, 3.0f);
		m_TextureData[index + 3].position = glmWorldtoView(m_Graph[drawcalls * maxVertices + j]) + glm::vec2(3.0f, 3.0f);

		if (IsVertexSelected(drawcalls * maxVertices + j)) {
			m_TextureData[index].color = pallete_selectedvertex;
			m_TextureData[index + 1].color = pallete_selectedvertex;
			m_TextureData[index + 2].color = pallete_selectedvertex;
			m_TextureData[index + 3].color = pallete_selectedvertex;
		}
		else {
			m_TextureData[index].color = pallete_vertex;
			m_TextureData[index + 1].color = pallete_vertex;
			m_TextureData[index + 2].color = pallete_vertex;
			m_TextureData[index + 3].color = pallete_vertex;
		}

		m_TextureData[index].texcoord = { 0,0 };
		m_TextureData[index + 1].texcoord = { 16,0 };
		m_TextureData[index + 2].texcoord = { 0,16 };
		m_TextureData[index + 3].texcoord = { 16,16 };

		index += 4;
	}
	m_Texture_vbo->InsertData(remainder * 4 * sizeof(TextureVertex), m_TextureData);
	m_Texture_vao->BindVertexBuffer(m_Texture_vbo);
	m_Texture_shader->Use();
	m_Texture_shader->SetUniformVec2("u_texSize", 16, 16);
	m_Texture_shader->SetUniformSampler2D("u_texture", 0);
	m_VertexTexture->Bind();
	m_Texture_vao->DrawElements(m_Texture_ibo, GL_TRIANGLES, 6 * remainder);
}

void cDisplay::DrawSelectionBox() {
	m_LineData[0].position = glm::vec2(currentmousepoint.x, currentmousepoint.y);
	m_LineData[1].position = glm::vec2(initialmousepoint.x, currentmousepoint.y);
	m_LineData[2].position = glm::vec2(initialmousepoint.x, currentmousepoint.y);
	m_LineData[3].position = glm::vec2(initialmousepoint.x, initialmousepoint.y);
	m_LineData[4].position = glm::vec2(initialmousepoint.x, initialmousepoint.y);
	m_LineData[5].position = glm::vec2(currentmousepoint.x, initialmousepoint.y);
	m_LineData[6].position = glm::vec2(currentmousepoint.x, initialmousepoint.y);
	m_LineData[7].position = glm::vec2(currentmousepoint.x, currentmousepoint.y);

	for (int i = 0; i < 8; i++) {
		m_LineData[i].color = glmcolor(0.8f, 0.8f, 0.8f);
	}

	m_Line_vbo->InsertData(8 * sizeof(LineVertex), m_LineData);
	m_Line_vao->BindVertexBuffer(m_Line_vbo);
	m_Line_shader->Use();
	m_Line_vao->DrawArrays(GL_LINES);
}

//Event Handlers

void cDisplay::OnLeftMouseToggle(wxMouseEvent& event) {
	if (event.LeftIsDown()) {
		initialmousepoint = event.GetPosition();
		currentmousepoint = event.GetPosition();
		keyboardstatememory = event.GetModifiers();
		initial_display_pos = display_pos;

		switch (m_tool) {
		case Tool::View:
			break;
		case Tool::Create:
			if (!event.HasAnyModifiers()){
				m_Graph.AddVertex(ViewtoWorld(event.GetPosition()));
			}
			else if (keyboardstatememory == wxMOD_SHIFT) {
				sel_positions.clear();
				sel_positions.reserve(selected_vertices.size());
				for (auto vindex : selected_vertices) {
					sel_positions.push_back(m_Graph[vindex]);
				}
			}
			break;
		case Tool::Select:
			if (!event.HasAnyModifiers()){
				auto l_hovered = [&](glm::dvec2 v) {
					wxPoint display_v = wxWorldtoView(v);
					int dx = (display_v.x - currentmousepoint.x), dy = (display_v.y - currentmousepoint.y);
					return (dx*dx + dy*dy < 9);
				};
				auto h_iter = std::find_if(m_Graph.begin(),m_Graph.end(),l_hovered);
				if (h_iter != m_Graph.end())
					ToggleSelectVertex((uint32_t)(h_iter - m_Graph.begin()));
			}
			break;
		}
	}
	else {
		switch (m_tool) {
		case Tool::View:
			break;
		case Tool::Select:
			if (keyboardstatememory == wxMOD_SHIFT) {
				wxPoint mincorner = wxPoint(std::min(currentmousepoint.x, initialmousepoint.x), std::min(currentmousepoint.y, initialmousepoint.y));
				wxPoint maxcorner = wxPoint(std::max(currentmousepoint.x, initialmousepoint.x), std::max(currentmousepoint.y, initialmousepoint.y));
				auto isinbox = [&](const glm::dvec2& v) {
					wxPoint viewv = wxWorldtoView(v);
					return (mincorner.x <= viewv.x) && (viewv.x <= maxcorner.x) && (mincorner.y <= viewv.y) && (viewv.y <= maxcorner.y);
				};
				switch (m_selectmode) {
				case SelectMode::New:
					selected_vertices.clear();
					selected_vertices.reserve(m_Graph.size());
					for (uint32_t i = 0; i < m_Graph.size(); i++) {
						if (isinbox(m_Graph[i])) {
							selected_vertices.push_back(i);
						}
					}
					selected_vertices.shrink_to_fit();
					break;
				case SelectMode::Add:
					selected_vertices.reserve(m_Graph.size() - selected_vertices.size());
					for (uint32_t i = 0; i < m_Graph.size(); i++) {
						if (isinbox(m_Graph[i])) {
							SelectVertex(i);
						}
					}
					selected_vertices.shrink_to_fit();
					break;
				case SelectMode::Subtract:
					selected_vertices.reserve(m_Graph.size() - selected_vertices.size());
					for (uint32_t i = 0; i < m_Graph.size(); i++) {
						if (isinbox(m_Graph[i])) {
							DeselectVertex(i);
						}
					}
					selected_vertices.shrink_to_fit();
					break;
				case SelectMode::Toggle:
					selected_vertices.reserve(m_Graph.size() - selected_vertices.size());
					for (uint32_t i = 0; i < m_Graph.size(); i++) {
						if (isinbox(m_Graph[i])) {
							if (IsVertexSelected(i))
								DeselectVertex(i);
							else
								SelectVertex(i);
						}
					}
					selected_vertices.shrink_to_fit();
					break;
				case SelectMode::Intersect:
					std::vector<uint32_t> new_selection;
					new_selection.reserve(m_Graph.size());
					for (uint32_t i = 0; i < m_Graph.size(); i++) {
						if (isinbox(m_Graph[i]) && IsVertexSelected(i)) {
							new_selection.push_back(i);
						}
					}
					selected_vertices = new_selection;
					break;
				}
			}
			break;
		}

		initialmousepoint = wxPoint(-1, -1);
		currentmousepoint = event.GetPosition();
		keyboardstatememory = 0;
	}
	event.Skip();
}

void cDisplay::OnMouseMove(wxMouseEvent& event) {
	currentmousepoint = event.GetPosition();
	glm::dvec2 mousepointdifference = glm::dvec2(initialmousepoint.x - currentmousepoint.x, currentmousepoint.y - initialmousepoint.y);

	if (event.LeftIsDown() && m_tool == Tool::View) {
		display_pos = initial_display_pos + display_scale * mousepointdifference;
	}
	switch (keyboardstatememory) {
	case wxMOD_CMD:
		display_pos = initial_display_pos + display_scale * mousepointdifference;
		break;
	case wxMOD_SHIFT:
		if (m_tool == Tool::Create) {
			for (int i = 0; i < sel_positions.size(); i++) {
				m_Graph.MoveVertex(selected_vertices[i], sel_positions[i] - display_scale * mousepointdifference);
			}
		}
		break;
	}
	event.Skip();
}

void cDisplay::OnMouseWheel(wxMouseEvent& event) {
	double wheelaction = event.GetWheelRotation() / event.GetWheelDelta();
	//The grid cannot be allowed to be zoomed in or out past certain limits due to double floating point limitations.
	double lowerlim = std::max(abs(display_pos.x), abs(display_pos.y))/ pow(2,30);
	double upperlim = pow(2, 1000);

	display_scale = clamp(display_scale * exp(-wheelaction / 3), lowerlim, upperlim);
	event.Skip();
}

void cDisplay::OnWindowResize(wxSizeEvent& event) {
	glViewport(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
	m_ProjMat = glm::ortho(0.0f, (float)CANVAS_WIDTH, (float)CANVAS_HEIGHT, 0.0f);
	m_Line_shader->SetUniformMat4("u_ProjMat", m_ProjMat);
	m_Texture_shader->SetUniformMat4("u_ProjMat", m_ProjMat);
	event.Skip();
}

void cDisplay::ToggleGrid()
{
	show_grid = !show_grid;
}
void cDisplay::ResetView()
{
	display_pos = { 0.0,0.0 };
	display_scale = 1;
}

cDisplayTimer::cDisplayTimer(cDisplay* display) : wxTimer()
{
	m_display = display;
}

void cDisplayTimer::Notify()
{
	m_display->Refresh();
}
