function navbar()
{
    var overlay = document.getElementById("mobile-nav-overlay");
    if (overlay.style.display == "none")
        overlay.style.display = "block";
    else
        overlay.style.display = "none";
}