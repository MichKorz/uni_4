/*jslint browser */
/*global window, document */

(function () {
    "use strict";

    var hamburger = document.getElementById("hamburger");
    var topnav = document.getElementById("topnav");
    var sideMenu = document.getElementById("sideMenu");
    var pageContent = document.getElementById("page-content");

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
        var gallery = document.getElementById("image_gallery");
        var images = Array.prototype.slice.call(gallery.getElementsByTagName("img"));

        preloadImages(images).then(function () {
            gallery.style.display = "grid";
        }).catch(function (error) {
            window.console.error(error);
        });
    }

    hamburger.addEventListener("click", function () {
        sideMenu.classList.toggle("open");
        pageContent.classList.toggle("moved");
        hamburger.classList.toggle("open");
    });

    document.addEventListener("DOMContentLoaded", function () {
        hamburger.style.display = "inline-block"; // Show hamburger
        topnav.style.display = "none"; // Hide classic topnav
    });

    window.addEventListener("load", function () {
        var galleryButton = document.getElementById("gallery-door");

        galleryButton.addEventListener("click", function () {
            showGallery();
        });

        document.getElementById("close_gallery").addEventListener("click", function () {
            document.getElementById("image_gallery").style.display = "none";
        });
    });
}());
