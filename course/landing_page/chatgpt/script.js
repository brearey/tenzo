const burger = document.getElementById("burger");
const navLinks = document.querySelector(".nav-links");

burger.addEventListener("click", () => {
  navLinks.classList.toggle("active");
});

document.querySelectorAll(".nav-links a").forEach(link => {
  link.addEventListener("click", () => {
    navLinks.classList.remove("active");
  });
});

const observer = new IntersectionObserver(entries => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      entry.target.classList.add("visible");
    }
  });
}, {
  threshold: 0.15
});

document.querySelectorAll(
  ".section, .card, .team-card, .pricing-card, .audience-card"
).forEach(el => {
  el.classList.add("fade-in");
  observer.observe(el);
});

const form = document.getElementById("registerForm");

form.addEventListener("submit", event => {
  event.preventDefault();

  alert(
    "Спасибо! Ваша заявка отправлена. Мы свяжемся с вами в ближайшее время."
  );

  form.reset();
});