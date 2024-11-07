#include <iostream>
#include <raylib.h>

std::string rl_col_to_str(Color col){
    auto col_eq = [](Color f, Color s){ return f.a == s.a && f.r == s.r && f.b == s.b && f.g == s.g; };
    if(col_eq(col, LIGHTGRAY)){
		return std::string("LIGHTGRAY");
	}else if(col_eq(col, GRAY)){
		return std::string("GRAY");
	}else if(col_eq(col, DARKGRAY)){
		return std::string("DARKGRAY");
	}else if(col_eq(col, YELLOW)){
		return std::string("YELLOW");
	}else if(col_eq(col, GOLD)){
		return std::string("GOLD");
	}else if(col_eq(col, ORANGE)){
		return std::string("ORANGE");
	}else if(col_eq(col, PINK)){
		return std::string("PINK");
	}else if(col_eq(col, RED)){
		return std::string("RED");
	}else if(col_eq(col, MAROON)){
		return std::string("MAROON");
	}else if(col_eq(col, GREEN)){
		return std::string("GREEN");
	}else if(col_eq(col, LIME)){
		return std::string("LIME");
	}else if(col_eq(col, DARKGREEN)){
		return std::string("DARKGREEN");
	}else if(col_eq(col, SKYBLUE)){
		return std::string("SKYBLUE");
	}else if(col_eq(col, BLUE)){
		return std::string("BLUE");
	}else if(col_eq(col, DARKBLUE)){
		return std::string("DARKBLUE");
	}else if(col_eq(col, PURPLE)){
		return std::string("PURPLE");
	}else if(col_eq(col, VIOLET)){
		return std::string("VIOLET");
	}else if(col_eq(col, DARKPURPLE)){
		return std::string("DARKPURPLE");
	}else if(col_eq(col, BEIGE)){
		return std::string("BEIGE");
	}else if(col_eq(col, BROWN)){
		return std::string("BROWN");
	}else if(col_eq(col, DARKBROWN)){
		return std::string("DARKBROWN");
	}else if(col_eq(col, WHITE)){
		return std::string("WHITE");
	}else if(col_eq(col, BLACK)){
		return std::string("BLACK");
	}else if(col_eq(col, BLANK)){
		return std::string("BLANK");
	}else if(col_eq(col, MAGENTA)){
		return std::string("MAGENTA");
	}else if(col_eq(col, RAYWHITE)){
		return std::string("RAYWHITE");
	}

    return std::string("UNKNOWN");
}
