#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h> 
#include <sys/stat.h>

#include <gtk/gtk.h>

#define LEFT 65361
#define DOWN 65364
#define RIGHT 65363
#define UP 65362

struct stc {	
	GtkWidget *window;
	GSource *to;
	GtkEventController* key;
	GtkWidget *area;
	
	char *text;
	int axis;	
	int speed;
	int score;
	int level;
	int go;
	int snakelen;
	double scoord [100] [2];
	double tempcoord[100] [2];
	double jcoord[2];
	int errflag;
	int statflag;
	guint tag;
	
};

gboolean timeout_callback (gpointer user_data);

gboolean startg(gpointer user_data){

	struct stc *s = user_data;

	s->speed = 4;
	s->level = 1;
	s->statflag = 0;
	s->axis = 3; //right
	s->score = 0;
	s->go = 1;
	s->snakelen = 3;
	s->text = NULL;
	
	for(int i =0; i < (s->snakelen); i++){
		s->scoord [i] [0] = 400 - (i*40);
		s->scoord [i] [1] = 400;

	}
	
	s->jcoord [0] = 200;
	s->jcoord [1] = 200;
	
	s->to= NULL;
	s->to = g_timeout_source_new(600 - (s->speed*100));
	s->tag = g_source_attach(s->to, NULL);
	g_source_set_callback(s->to, timeout_callback, s, NULL);

return 0;
}


static void draw_function (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {

	struct stc *s = user_data;
		
	//cairo_surface_t *surface;
	if(s->level == 0 || s->level == 1){
	cairo_set_source_rgb (cr, 0.40, 0.80, 0.70); /* sg */
	}
	else if(s->level == 2){
	cairo_set_source_rgb (cr, 0.9, 0.90, 0.00); /* yel */
	}
	else if(s->level == 3){
	cairo_set_source_rgb (cr, 0.90, 0.00, 0.90); /* pp */
	}
	else if(s->level == 4){
	cairo_set_source_rgb (cr, 0.00, 0.00, 0.00); /* bl*/
	}
	cairo_paint (cr);
    cairo_stroke (cr);
      

	for (int i = 0 ; i < (s->snakelen); i++) {
			
			cairo_set_source_rgba(cr, 0.0, 0.5, 1.0, 0.8);
			
			cairo_rectangle(cr, s->scoord [i] [0] , s->scoord [i] [1], 40, 40);
			
			cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
			
			cairo_rectangle(cr, s->scoord [i] [0] + 5 , s->scoord [i] [1] - 5, 40, 40);
		
			cairo_fill(cr);  
	}  
	
	for (int i = 0 ; i < 20; i++) {
			
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		for(int j = 0 ; j < 20; j++){
		
			if(i == 0){
				cairo_rectangle(cr, j*40 , i*40, 40, 5); //upper
				
				cairo_rectangle(cr, i*40 , (j-1)*40, 5, 40); // left
			}
			else if (i == 19){
				cairo_rectangle(cr, j*40 , i*40, 40, 5); // down
				
				cairo_rectangle(cr, 795 , (j-1)*40, 5, 40); //right
			}
		
		}		
		
		cairo_fill(cr);  
	}  
		
		
	cairo_set_source_rgba(cr, 0.9, 0.0, 0.0, 0.8);
	cairo_rectangle(cr, s->jcoord[0] , s->jcoord[1], 40, 40);

	cairo_fill(cr); 
	 
	cairo_move_to(cr, 5.0, 30.0);
	
    int cntsz;
    
	
    cntsz = snprintf(NULL, 0, "%d", s->score);
	s->text = alloca(cntsz + 1);
    snprintf(s->text, cntsz + 1, "%d", s->score);
   
	
	cairo_show_text (cr, "SCORE:");
	cairo_show_text (cr, s->text);
	
	cairo_stroke (cr);	
    
}

/*
static void
draw_functionq (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
	cairo_arc (cr, width / 2.0, height / 2.0, MIN (width, height) / 2.0, 0, 2 * G_PI);
}
*/
gboolean timeout_callback (gpointer user_data)
{

	struct stc *s = user_data;
	
	s->statflag = 0;
			
	if(s->jcoord[0] == s->scoord [0] [0] && s->jcoord[1] == s->scoord [0] [1]){

		s->score++;
		s->snakelen++;
				
		if(s->score == 5 && s->level == 1){
			s->speed++;		
		    //s->to = g_timeout_source_new(600 - (s->speed*100));
			//g_source_attach(s->to, NULL);
			//g_source_set_callback(s->to, timeout_callback, s, NULL);
			s->level = 2;	
			
		}
		if(s->score == 10 && s->level == 2){
			s->speed++;	
		//	s->to = g_timeout_source_new(600 - (s->speed*100));
		//	g_source_attach(s->to, NULL);
		//	g_source_set_callback(s->to, timeout_callback, s, NULL);
			s->level = 3;
			
		}
			
		if(s->score == 20 && s->level == 3){
			s->speed++;	
		//	s->to = g_timeout_source_new(100);
		//	g_source_attach(s->to, NULL);
		//	g_source_set_callback(s->to, timeout_callback, s, NULL);
			s->level = 4;
		
		}

		s->jcoord[0] = g_random_int_range(1,20)*40;
		s->jcoord[1] = g_random_int_range(1,19)*40;
		
		s->errflag = 0;
		
		do{
			for(int i = 0; i < s->snakelen; i++){
		
				if(s->jcoord[0] == s->scoord [i] [0] && s->jcoord[1] == s->scoord [i] [1]){
					s->errflag = 1;
					g_print("RANDOM \n");
					s->jcoord[0] = g_random_int_range(1,20)*40;
					s->jcoord[1] = g_random_int_range(1,19)*40;
				}
				else{
					s->errflag = 0;
				}
			}
		}
		
		while(s->errflag == 1);
	}

	for(int i = 0; i < s->snakelen ; i++){
	
		if(s->go == 1){
			s->tempcoord[i] [0] = s->scoord [i] [0];
			s->tempcoord[i] [1] = s->scoord [i] [1];
		}
				
	}
		
	if (s->axis == 0 && s->go == 1){  //up
		s->scoord [0] [1] = s->scoord [0] [1] - 40;
	}
	else if (s->axis == 1 && s->go == 1){ //down
		s->scoord [0] [1] = s->scoord [0] [1] + 40;
	}
	else if (s->axis == 2 && s->go == 1){  //left
		s->scoord [0] [0] = s->scoord [0] [0] - 40;
	}
	else if (s->axis == 3 && s->go == 1){  //right
		s->scoord [0] [0] = s->scoord [0] [0] + 40;
	}
		
	if( s->scoord [0] [0] < 0 || s->scoord [0] [1] < 0 || s->scoord [0] [0] > 760 ||s->scoord [0] [1] > 720){
		
		s->go = 0;
		g_print("END \n");
		poll(NULL, 0, 1000);
		g_source_remove (s->tag);
		startg(s);

	}
		
	for(int i = 1; i < s->snakelen ; i++){
		if(s->go == 1){
			s->scoord [i] [0] = s->tempcoord[i - 1] [0];
			s->scoord [i] [1] = s->tempcoord[i - 1] [1];
		}
		
	}
	
	for(int i = 1; i < s->snakelen; i++){
		if( s->scoord [0] [0] == s->scoord [i] [0] && s->scoord [0] [1] == s->scoord [i] [1]){
		
			s->go = 0;
			
			g_print("END \n");
			poll(NULL, 0, 1000);
			g_source_remove (s->tag);
			startg(s);
								
		}
	}

	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (s->area), draw_function, s, NULL);
	
	if(s->go == 0){
		g_source_remove (s->tag);
		return 0;
	}
	else{
		return 1;
	}
}

gboolean key_pressed (GtkEventControllerKey* self,guint keyval,guint keycode, GdkModifierType state,gpointer user_data)
{
	
	struct stc *s = user_data;
 	
 	if (s->statflag == 0){
		if (keyval == LEFT || keyval == 'a'){
			if(s->axis != 3){
				//g_print(" LEFT ");
				s->axis = 2;
			}
		}
		else if (keyval == UP || keyval == 'w'){
			if(s->axis != 1){
				//g_print(" UP ");
				s->axis = 0;
				}
		}
		else if (keyval == RIGHT || keyval == 'd'){
			if(s->axis != 2){
			//g_print(" RIGHT ");
			s->axis = 3;
			}
		}
		else if (keyval == DOWN || keyval == 's'){
			if(s->axis != 0){
			//g_print(" DOWN " );
			s->axis = 1;
			}
		}
		s->statflag = 1;
	}

	return 0;
}

static void activate (GApplication *app, gpointer user_data) {

	struct stc *s = user_data;

	s->window = gtk_application_window_new(GTK_APPLICATION (app));
  
	gtk_window_set_resizable (GTK_WINDOW (s->window),FALSE);
	
	s->area = gtk_drawing_area_new ();
	
	s->key = gtk_event_controller_key_new();
	
    gtk_window_set_title(GTK_WINDOW(s->window), "new");
	gtk_widget_set_size_request(s->window, 800, 800);
	//gtk_widget_set_focusable (s->window ,TRUE );
	//gtk_window_set_default_size(GTK_WINDOW(s->window),600,600);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (s->area), draw_function, s, NULL);
	gtk_window_set_child (GTK_WINDOW (s->window), s->area);
			
	//g_timeout_add(600 - (s->speed*100),timeout_callback,s);
	
	g_signal_connect (s->key, "key-pressed",G_CALLBACK(key_pressed), s);
	
	gtk_widget_add_controller (s->window,s->key);

	gtk_widget_show (s->window);
	
}

int main(int argc, char **argv)
{  
	
	struct stc *s = (struct stc *)malloc(sizeof(struct stc));
	memset(s, 0, sizeof(struct stc));
		
	startg(s);
		
	GtkApplication *app = gtk_application_new(NULL,G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"activate", G_CALLBACK(activate), s);
	g_application_run(G_APPLICATION (app), argc,argv);
	

	g_object_unref(app);
	
	free(s);
	
	return 0;
	
}
