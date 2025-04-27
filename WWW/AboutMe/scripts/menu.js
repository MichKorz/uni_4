const hamburger = document.getElementById("hamburger");
const topnav = document.getElementById("topnav");
const sideMenu = document.getElementById("sideMenu");
const pageContent = document.getElementById("page-content");

hamburger.addEventListener("click", () => {
    sideMenu.classList.toggle("open");
    pageContent.classList.toggle("moved");
    hamburger.classList.toggle("open");
});

// When JS is running:
document.addEventListener("DOMContentLoaded", () => {
    hamburger.style.display = "inline-block"; // Show hamburger
    topnav.style.display = "none"; // Hide classic topnav
});


(function () {
    "use strict";

    function preloadImages(imageElements) {
        var promises = [];

        imageElements.forEach(function (img) {
            promises.push(new Promise(function (resolve, reject) {
                if (img.complete) {
                    resolve();
                } else {
                    img.addEventListener("load", function () {
                        resolve();
                    }, { once: true });

                    img.addEventListener("error", function () {
                        reject(new Error("Image failed to load: " + img.src));
                    }, { once: true });
                }
            }));
        });

        return Promise.all(promises);
    }

    function showGallery() {
        var gallery = document.getElementById("image_gallery"),
            images = Array.prototype.slice.call(gallery.getElementsByTagName("img"));

        preloadImages(images).then(function () {
            gallery.style.display = "grid";
        }).catch(function (error) {
            window.console.error(error);
        });
    }

    window.addEventListener("load", function () {
        var galleryButton = document.getElementById("gallery-door");

        galleryButton.addEventListener("click", function () {
            showGallery();
        });
    });
}());