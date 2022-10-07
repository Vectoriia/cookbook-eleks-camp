set(RESOURCES
    entries.txt
)
set(IMAGES 
    images/welcomepage.png
    images/window_icon.png)
                
install(FILES ${RESOURCES} DESTINATION app/data/ COMPONENT desktop)
install(FILES ${IMAGES} DESTINATION app/data/images COMPONENT desktop)