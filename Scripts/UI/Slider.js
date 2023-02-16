//-----------------------------------------------------------------------------------------------------------//
//   Cursed hack to use an invisible slider to resize the main containers and have it float between them     //
//   so that it looks like the user is resizing the containers through an actual full height resize handle   //
//-----------------------------------------------------------------------------------------------------------//





// Save slider element
let slider;


var ui_slider = {
    // Updates the maximum and minimum values of the slider. Used in browser zoom and window resize
    update_range : function(){
        slider.min = document.getElementById("CALC-SLIDER-MIN").clientWidth;
        slider.max = document.getElementById("CALC-VW"        ).clientWidth - document.getElementById("CALC-SLIDER-MAX").clientWidth;
    },



    // Stolen function
    getBackgroundSize : function(elem) {
        // Ui_slider.
        //       * Gets elem computed styles:
        //             - CSS background-size
        //             - element's width and height
        //       * Extracts background URL
        let computedStyle = getComputedStyle(elem),
            image = new Image(),
            src = computedStyle.backgroundImage.replace(/url\((['"])?(.*?)\1\)/gi, '$2'),
            cssSize = computedStyle.backgroundSize,
            elemW = parseInt(computedStyle.width.replace('px', ''), 10),
            elemH = parseInt(computedStyle.height.replace('px', ''), 10),
            elemDim = [elemW, elemH],
            computedDim = [],
            ratio;
        // Load the image with the extracted URL.
        // Should be in cache already.
        image.src = src;
        // Determine the 'ratio'
        ratio = image.width > image.height ? image.width / image.height : image.height / image.width;
        // Split background-size properties into array
        cssSize = cssSize.split(' ');
        // First property is width. It is always set to something.
        computedDim[0] = cssSize[0];
        // If height not set, set it to auto
        computedDim[1] = cssSize.length > 1 ? cssSize[1] : 'auto';
        if(cssSize[0] === 'cover') {
            // Width is greater than height
            if(elemDim[0] > elemDim[1]) {
                // Elem's ratio greater than or equal to img ratio
                if(elemDim[0] / elemDim[1] >= ratio) {
                    computedDim[0] = elemDim[0];
                    computedDim[1] = 'auto';
                } else {
                    computedDim[0] = 'auto';
                    computedDim[1] = elemDim[1];
                }
            } else {
                computedDim[0] = 'auto';
                computedDim[1] = elemDim[1];
            }
        } else if(cssSize[0] === 'contain') {
            // Width is less than height
            if(elemDim[0] < elemDim[1]) {
                computedDim[0] = elemDim[0];
                computedDim[1] = 'auto';
            } else {
                // Elem's ratio is greater than or equal to img ratio
                if(elemDim[0] / elemDim[1] >= ratio) {
                    computedDim[0] = 'auto';
                    computedDim[1] = elemDim[1];
                } else {
                    computedDim[1] = 'auto';
                    computedDim[0] = elemDim[0];
                }
            }
        } else {
            // If not 'cover' or 'contain', loop through the values
            for(let i = cssSize.length; i--;) {
                // Check if values are in pixels or in percentage
                if (cssSize[i].indexOf('px') > -1) {
                    // If in pixels, just remove the 'px' to get the value
                    computedDim[i] = cssSize[i].replace('px', '');
                } else if (cssSize[i].indexOf('%') > -1) {
                    // If percentage, get percentage of elem's dimension
                    // And assign it to the computed dimension
                    computedDim[i] = elemDim[i] * (cssSize[i].replace('%', '') / 100);
                }
            }
        }
        // If both values are set to auto, return image's
        // Original width and height
        if(computedDim[0] === 'auto' && computedDim[1] === 'auto') {
            computedDim[0] = image.width;
            computedDim[1] = image.height;
        } else {
            // Depending on whether width or height is auto,
            // Calculate the value in pixels of auto.
            // Ratio in here is just getting proportions.
            ratio = computedDim[0] === 'auto' ? image.height / computedDim[1] : image.width / computedDim[0];
            computedDim[0] = computedDim[0] === 'auto' ? image.width / ratio : computedDim[0];
            computedDim[1] = computedDim[1] === 'auto' ? image.height / ratio : computedDim[1];
        }
        // Finally, return an object with the width and height of the
        // Background image.
        return {
            width: computedDim[0],
            height: computedDim[1]
        };
    },




    // Updates the logo- and logo-top- width
    update_logo_width : function(value){
        document.querySelector("logo-top-").style.width = "calc(" +
            "100% - " + value + "px - var(--slider-w) - " + window.getComputedStyle(document.documentElement).getPropertyValue("--slider-w") +
        ")";
        let logos = document.querySelectorAll("logo-");
        for(let i = 0; i < logos.length; ++i){
            logos[i].style.backgroundPositionX = "calc(" +
                "calc(" +
                    "calc(100vw - var(--main-padding-r)) / 2 + " +
                    "calc(" + value + "px + var(--main-padding-r) + var(--main-padding-l)) / 2" +
                ") - " +
                "calc(" +
                    ui_slider.getBackgroundSize(logos[i]).width + "px / " +
                    "2" +
                ")" +
            ")";
        }
    },




    // Updates the width of the left and right main containers
    update_main_width : function(){
        document.getElementById("main-left").style.width  = "calc(" +
            slider.value +  "px - " +
            "var(--main-padding-l)" +
        ")";
        document.getElementById("main-right").style.width = "calc(" +
            "100% - " +
            slider.value +  "px - " +
            "var(--main-padding-l) - " +
            "var(--main-padding-r) * 2" +
        ")";
        ui_slider.update_logo_width(slider.value);
        window.localStorage.setItem("slider-value", slider.value);
        move_to_view("auto"); // Fix random scrolling caused by children resizing
    },

    // Update on browser zoom and window resize
    _onresize : function() {
        ui_slider.update_range();
        ui_slider.update_main_width();
    },


    init_slider_first_time : function(){
        window.localStorage.setItem("slider-set", "set");
        let min = document.getElementById("CALC-SLIDER-MIN").clientWidth;
        let max = document.getElementById("CALC-VW"        ).clientWidth - document.getElementById("CALC-SLIDER-MAX").clientWidth;
        window.localStorage.setItem("slider-value", (min + max) / 2);
    },
    // Set slider value after page refresh and initialize it if needed
    init_slider : function(){
        if(window.localStorage.getItem("slider-set") != "set") {
            ui_slider.init_slider_first_time();
        }
        slider.addEventListener("onmouseup", ui_slider.update_main_width);
        slider.value = window.localStorage.getItem("slider-value");
    },







    //TODO maybe fix this mess of an hacked contraption thingie
    init : function() {
        // Save slider element in global variable
        slider = document.getElementById("main-slider");


        // Calculate the minimum value of the slider
        let min = document.createElement('div');
        min.style.position = "fixed";
        min.id = "CALC-SLIDER-MIN";
        min.style.minWidth = "calc(var(--main-padding-l))";
        min.style.maxWidth = "calc(var(--main-padding-l))";
        document.body.appendChild(min);


        // Calculate the maximum value of the slider (starts from the right)
        let max = document.createElement('div');
        max.style.position = "fixed";
        max.id = "CALC-SLIDER-MAX";
        max.style.minWidth = "calc(var(--main-padding-r) * 2 + var(--slider-w))";
        max.style.maxWidth = "calc(var(--main-padding-r) * 2 + var(--slider-w))";
        document.body.appendChild(max);


        // Calculate the workspace width in pixels
        let vw = document.createElement('div');
        vw.style.position = "fixed";
        vw.style.minWidth = "100vw";
        vw.style.maxWidth = "100vw";
        vw.id = "CALC-VW";
        document.body.appendChild(vw);


        // Run initializer functions
        ui_slider.init_slider();
        ui_slider.update_range();
        ui_slider.update_main_width();
    }
}