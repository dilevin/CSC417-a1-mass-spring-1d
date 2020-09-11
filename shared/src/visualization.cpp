#include <visualization.h> 

//libigl viewer
namespace Visualize {

igl::opengl::glfw::Viewer g_viewer;
igl::opengl::glfw::imgui::ImGuiMenu menu;
 
//meshes in the scene 
std::vector<std::pair<Eigen::MatrixXd, Eigen::MatrixXi> > g_geometry;
std::vector<unsigned int> g_id; //id into libigl for these meshes 

//pointers to q and qdot
Eigen::VectorXd const *g_q;
Eigen::VectorXd const *g_qdot;

//cache for phase space data 
std::deque<std::pair<float, float> > g_state;
}

igl::opengl::glfw::Viewer & Visualize::viewer() { return g_viewer; }

bool Visualize::plot_phase_space(const char *label, ImVec2 q_bounds, ImVec2 q_dot_bounds, const Eigen::VectorXd &q, const Eigen::VectorXd &q_dot) {
        
        using namespace ImGui; 

        unsigned int cache_size = 10000;
        unsigned int num_lines = 5; //always odd number because I want lines to cross at 0,0

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        if(g_state.size() > cache_size) {
            g_state.pop_front();
        }

        //update plotting cache 
        g_state.push_back(std::make_pair(q(0), q_dot(0)));

        //ImGUI stuff that I don't understand (taken from code example here: https://github.com/ocornut/imgui/issues/786)
        const ImGuiStyle& Style = GetStyle();
        const ImGuiIO& IO = GetIO();
        ImDrawList* DrawList = GetWindowDrawList();
        ImGuiWindow* Window = GetCurrentWindow();
        
        if (Window->SkipItems)
            return false;

        // header and spacing
        int hovered = IsItemActive() || IsItemHovered(); // IsItemDragged() ?
        Dummy(ImVec2(0,3));

        // prepare canvas
        ImVec2 avail = GetContentRegionAvail();
        ImVec2 Canvas(ImMin(avail.x, avail.y), ImMin(avail.x, avail.y));

        Canvas = CalcItemSize(Canvas, style.FramePadding.x * 2.0f, style.FramePadding.y * 2.0f);
        ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);

        const ImGuiID id = Window->GetID(label);
        
        RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);
        
        //local grid coordinates are -1,1 in both directions
        auto pix_to_normalized =  [&bb, &Canvas](ImVec2 pix) {  return ImVec2((pix.x-bb.Min.x)/Canvas.x,(pix.y-bb.Min.y)/Canvas.y); };
        auto normalized_to_pix =  [&bb, &Canvas] (ImVec2 norm) {  return ImVec2(norm.x*Canvas.x + bb.Min.x, norm.y*Canvas.y + bb.Min.y); };
        auto data_to_normalized =  [&q_bounds, &q_dot_bounds] (ImVec2 state) {  return ImVec2((state.x - q_bounds.x)/(q_bounds.y - q_bounds.x), (state.y - q_dot_bounds.x)/(q_dot_bounds.y - q_dot_bounds.x)); };
        
        //background grid centered on origin
        for (float i = 0.f; i <= 1.f; i+= 1.f/static_cast<float>(num_lines-1)) {
            DrawList->AddLine(
                normalized_to_pix(ImVec2(i, 0.f)),
                normalized_to_pix(ImVec2(i, 1.f)),
                GetColorU32(ImGuiCol_TextDisabled), 1.2);
        }   
        
        for (float i = 0.f; i <= 1.f; i+= 1.f/static_cast<float>(num_lines-1)) {
            DrawList->AddLine(
                normalized_to_pix(ImVec2(0.f, i)),
                normalized_to_pix(ImVec2(1.f, i)),
                GetColorU32(ImGuiCol_TextDisabled), 1.2);
        }  

        //plot phase space trajectory
        bool clip_p1;
        bool clip_p2;
        for(unsigned int i=0; i<g_state.size()-1; ++i) {
            
            clip_p1 = false;
            clip_p2 = false;

            ImVec2 p1 = data_to_normalized(ImVec2(g_state[i].first, g_state[i].second));
            ImVec2 p2 = data_to_normalized(ImVec2(g_state[i+1].first, g_state[i+1].second));

            if(p1.x < 0.f || p1.x > 1.f || p1.y < 0.f || p1.y > 1.f) {
                clip_p1 = true;
            }

            if(p2.x < 0.f || p2.x > 1.f || p2.y < 0.f || p2.y > 1.f) {
                clip_p2 = true;
            }

            p1.x = ImMin(ImMax(p1.x,0.f),1.f);
            p1.y = ImMin(ImMax(p1.y,0.f),1.f);
            p2.x = ImMin(ImMax(p2.x,0.f),1.f);
            p2.y = ImMin(ImMax(p2.y,0.f),1.f);

            if(!clip_p1 || !clip_p2) {
                DrawList->AddLine(
                    normalized_to_pix(p1),
                    normalized_to_pix(p2),
                    GetColorU32(ImGuiCol_ColumnActive), 2);
            }
            //std::cout<<data_to_normalized(ImVec2(g_state[i].first, g_state[i].second)).x<<"\n";
        }

        //label axes 

        return true;

    }
                       
    void Visualize::setup(const Eigen::VectorXd &q, const Eigen::VectorXd &qdot) {

        g_q = &q;
        g_qdot = &qdot;

        //add new menu for phase space plotting
        Visualize::g_viewer.plugins.push_back(&menu);

        menu.callback_draw_viewer_menu = [&]()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 5.3f;
            style.FrameRounding = 2.3f;
            style.ScrollbarRounding = 0;

            style.Colors[ImGuiCol_Text]                  = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
            style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);
            style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
            style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
            style.Colors[ImGuiCol_Border]                = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
            style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 0.00f, 0.00f, 0.00f);
            style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
            style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
            style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.960f, 0.960f, 0.960f, 1.0f);
            style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.960f, 0.960f, 0.960f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.960f, 0.960f, 0.960f, 1.0f);
            style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
            style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
            style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
            style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
            style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
            style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
            style.Colors[ImGuiCol_Button]                = ImVec4(0.48f, 0.72f, 0.89f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.50f, 0.69f, 0.99f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
            style.Colors[ImGuiCol_Header]                = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_Column]                = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
            style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
            style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
            style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
            style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
            style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
            style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

            // Draw parent menu content
            menu.draw_viewer_menu();
        };

        // Add content to the default menu window
          menu.callback_draw_custom_window = [&]()
        {
            // Define next window position + size
            ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling(), 10), ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiSetCond_FirstUseEver);
            ImGui::Begin(
                "Phase Space Plot", nullptr,
                ImGuiWindowFlags_NoSavedSettings

            );

            ImVec2 min = ImGui::GetWindowContentRegionMin();
            ImVec2 max = ImGui::GetWindowContentRegionMax();

            max.x = ( max.x - min.x ) / 2;
            max.y -= min.y + ImGui::GetItemsLineHeightWithSpacing() * 3;

            Visualize::plot_phase_space("example", ImVec2(-15,15), ImVec2(-15,15), *g_q, *g_qdot);

            ImGui::End();
        };

        Visualize::g_viewer.core().background_color.setConstant(1.0);
        Visualize::g_viewer.core().is_animating = true;
    }

    void Visualize::add_object_to_scene(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::RowVector3d color) {

        //add mesh to libigl and store id for access later
        if(g_geometry.size() == 0) {
            g_id.push_back(0);     
        } else {
            g_id.push_back(g_viewer.append_mesh());
        }

        g_viewer.data().set_mesh(V,F);
        g_viewer.data().set_colors(color);

        //add mesh to geometry vector
        g_geometry.push_back(std::make_pair(V,F));

    }

    void Visualize::rigid_transform_1d(unsigned int id, double x) {
        
        //reset vertex positions 
        for(unsigned int ii=0; ii<g_geometry[id].first.rows(); ++ii) {
            g_viewer.data_list[g_id[id]].V(ii,0) = g_geometry[id].first(ii,0) + x;
         }

         //tell viewer to update
         g_viewer.data_list[g_id[id]].dirty |= igl::opengl::MeshGL::DIRTY_POSITION;
    }

    void Visualize::scale_x(unsigned int id, double x) {
        
        //reset vertex positions 
        for(unsigned int ii=0; ii<g_geometry[id].first.rows(); ++ii) {
            g_viewer.data_list[g_id[id]].V(ii,0) = x*g_geometry[id].first(ii,0);
         }

         //tell viewer to update
         g_viewer.data_list[g_id[id]].dirty |= igl::opengl::MeshGL::DIRTY_POSITION;

    }