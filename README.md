# SimpleOpenGLStarterClass
Simple starter class for games, app and etc. written in C++ and OpenGL!

# DEMO:

```#define STB_IMAGE_IMPLEMENTATION
#include "imcgknEngine.hpp"

using namespace imcgkn;

int main()
{
	try
	{
		std::unique_ptr<Window> window = std::make_unique<Window>(800, 600, "Hello World", 4, 5, WindowFlags::Shown | WindowFlags::Resizable);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>("vertex.glsl", "fragment.glsl", "");

		std::vector< Vertex> vertices = {
			{ { -0.5f,-0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
			{ {  0.5f,-0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
			{ {  0.0f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 1.0f } }
		};
		std::vector<unsigned int> indices = {
			0, 1, 2
		};

		std::shared_ptr<Renderable> renderable = std::make_shared<Renderable>(vertices, BufferUsage::StaticDraw, indices, BufferUsage::StaticDraw);

		std::unique_ptr<GameObject> go1 = std::make_unique<GameObject>(-50.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 45.0f);
		std::unique_ptr<GameObject> go2 = std::make_unique<GameObject>(50.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f);

		go1->CreateRenderable(vertices, BufferUsage::StaticDraw, indices, BufferUsage::StaticDraw);
		go2->SetRenderable(renderable);

		glm::mat4 projection = glm::ortho(-window->GetWidth() / 2.0f, window->GetWidth() / 2.0f, -window->GetHeight() / 2.0f, window->GetHeight() / 2.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		shader->SetMat4("projection", 1, GL_FALSE, projection);
		shader->SetMat4("view", 1, GL_FALSE, view);

		while (window->IsOpen())
		{
			window->UpdateDeltaTime();
			window->FirstUpdate();

			if (window->CheckKeyDown(SDL_SCANCODE_X))
				std::cout << "X is being pressed !\n";
			if (window->CheckKeyUp(SDL_SCANCODE_X))
				std::cout << "X was released !\n";

			window->SecondUpdate();

			projection = glm::ortho(-window->GetWidth() / 2.0f, window->GetWidth() / 2.0f, -window->GetHeight() / 2.0f, window->GetHeight() / 2.0f, -1.0f, 1.0f);

			shader->SetMat4("projection", 1, GL_FALSE, projection);

			window->ClearScreen(GL_COLOR_BUFFER_BIT);

			go1->Render(shader.get(), "model", "tex0", RenderMode::Triangles);
			go2->Render(shader.get(), "model", "tex0", RenderMode::Triangles);

			window->SwapBuffer();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

