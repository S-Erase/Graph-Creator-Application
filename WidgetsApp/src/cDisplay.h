#pragma once
#include "cPCH.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include "cOpenGLObjects.h"
#include "cGraph.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <wx/glcanvas.h>

using glmcolor = glm::vec3;

static wxColor glmtowxColor(const glmcolor& color) { return wxColor(255 * color.r, 255 * color.g, 255 * color.b); }
static glmcolor wxtoglmColor(const wxColor& color) { return glmcolor((float)color.Red()/255, (float)color.Green() / 255, (float)color.Blue() / 255); }

enum class Tool { View, Create, Select, Edit };

enum class SelectMode { New, Add, Subtract, Toggle, Intersect };

class cDisplay : public wxGLCanvas
{
public:
	cDisplay(wxWindow* parent);
	~cDisplay();

	inline cGraph& GetGraph() { return m_Graph; }

	inline Tool GetTool() { return m_tool; }
	inline void SetTool(Tool newtool) { m_tool = newtool; }

	//Select
	void SelectVertex(uint32_t vindex);
	void DeselectVertex(uint32_t vindex);
	void ToggleSelectVertex(uint32_t vindex);
	bool IsVertexSelected(uint32_t vindex);
	void SelectAll();
	void DeselectAll();
	void ToggleSelection();
	void DeleteSelection();
	void SetSelectionMode(SelectMode mode);

	//Controls
	void ToggleGrid();
	void ResetView();

	//Pallete
	wxColor GetBGColor();
	wxColor GetGridColor();
	wxColor GetVertexColor();
	wxColor GetSelectedVertexColor();
	void SetBGColor(const wxColor& color);
	void SetGridColor(const wxColor& color);
	void SetVertexColor(const wxColor& color);
	void SetSelectedVertexColor(const wxColor& color);

private:
	void InitializeGLEW();
	void Setup();

	Tool m_tool = Tool::View;

	cGraph m_Graph;
	glm::dvec2 ViewtoWorld(const glm::vec2& viewv);
	glm::dvec2 ViewtoWorld(const wxPoint& viewv);
	glm::vec2 glmWorldtoView(const glm::dvec2& worldv);
	wxPoint wxWorldtoView(const glm::dvec2& worldv);

	//Render Commands
	void DrawGridLines();
	void DrawGridCoords();
	void DrawGraph();
	void DrawSelectionBox();

	//Event Handlers
	void OnPaint(wxPaintEvent& event);
	void OnLeftMouseToggle(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnWindowResize(wxSizeEvent& event);

private:
	//Selection
	SelectMode m_selectmode = SelectMode::New;
	std::vector<uint32_t> selected_vertices = {};
	std::vector<glm::dvec2> sel_positions = {};

	//Vertex Data Structures
	struct LineVertex {
		glm::vec2 position;
		glm::vec3 color;
	};

	struct TextureVertex {
		glm::vec2 position;
		glm::vec3 color;
		glm::vec2 texcoord;
	};

	//OopenGL Buffers
	wxGLContext m_Context;

	std::shared_ptr<cOpenGLDynamicVBO> m_Line_vbo;
	std::shared_ptr<cOpenGLShader> m_Line_shader;
	std::shared_ptr<cOpenGLVAO> m_Line_vao;

	std::shared_ptr<cOpenGLDynamicVBO> m_Texture_vbo;
	std::shared_ptr<cOpenGLShader> m_Texture_shader;
	std::shared_ptr<cOpenGLVAO> m_Texture_vao;
	std::shared_ptr<cOpenGLStaticIBO> m_Texture_ibo;

	std::shared_ptr<cOpenGLTexture> m_VertexTexture;
	std::shared_ptr<cOpenGLTexture> m_FontTexture;

	uint32_t maxVertices = 1000;
	LineVertex* m_LineData = new LineVertex[2 * maxVertices];
	TextureVertex* m_TextureData = new TextureVertex[4 * maxVertices];

	glm::mat4 m_ProjMat;

	//Mouse/Keyboard states
	wxPoint currentmousepoint = wxPoint(-1, -1);
	wxPoint initialmousepoint = wxPoint(-1, -1);
	int keyboardstatememory = 0; //Stores the keyboard state at the last mouse down event

	//Display variables
	glm::dvec2 display_pos = { 0, 0 };
	glm::dvec2 initial_display_pos = { 0, 0 };
	double display_scale = 1;
	double display_rot = 0;
	bool show_grid = true;

	//Palette
	glmcolor pallete_background = { 0.1f,0.4f,0.4f };
	glmcolor pallete_subgrid = { 0.325f,0.55f,0.55f };
	glmcolor pallete_grid = { 0.55f,0.7f,0.7f };
	glmcolor pallete_axes = { 1.0f,1.0f,1.0f };
	glmcolor pallete_vertex = { 1.0f,1.0f,1.0f };
	glmcolor pallete_selectedvertex = { 0.5f,0.85f,0.0f };
};

class cDisplayTimer : public wxTimer
{
public:
	cDisplayTimer(cDisplay* display);
	void Notify();
private:
	cDisplay* m_display;
};