/* =============================================
   ROBOHAPSAGAI — script.js
   ============================================= */

'use strict';

/* ── Smooth scroll for anchor links ── */
document.querySelectorAll('a[href^="#"]').forEach(link => {
  link.addEventListener('click', e => {
    const target = document.querySelector(link.getAttribute('href'));
    if (!target) return;
    e.preventDefault();
    target.scrollIntoView({ behavior: 'smooth', block: 'start' });
    // Close mobile menu if open
    mobileMenu.classList.remove('open');
    burger.classList.remove('active');
  });
});

/* ── Sticky nav ── */
const nav = document.getElementById('nav');
function updateNav() {
  nav.classList.toggle('scrolled', window.scrollY > 60);
}
window.addEventListener('scroll', updateNav, { passive: true });
updateNav();

/* ── Mobile burger ── */
const burger = document.getElementById('burger');
const mobileMenu = document.getElementById('mobileMenu');
burger.addEventListener('click', () => {
  const open = mobileMenu.classList.toggle('open');
  burger.classList.toggle('active', open);
});
// Close on outside click
document.addEventListener('click', e => {
  if (!burger.contains(e.target) && !mobileMenu.contains(e.target)) {
    mobileMenu.classList.remove('open');
    burger.classList.remove('active');
  }
});

/* ── Reveal on scroll (Intersection Observer) ── */
const revealEls = document.querySelectorAll('.reveal');
const revealObserver = new IntersectionObserver(
  entries => {
    entries.forEach((entry, i) => {
      if (entry.isIntersecting) {
        // Stagger siblings in the same parent
        const siblings = [...entry.target.parentElement.querySelectorAll('.reveal')];
        const idx = siblings.indexOf(entry.target);
        entry.target.style.transitionDelay = `${idx * 60}ms`;
        entry.target.classList.add('visible');
        revealObserver.unobserve(entry.target);
      }
    });
  },
  { threshold: 0.12, rootMargin: '0px 0px -40px 0px' }
);
revealEls.forEach(el => revealObserver.observe(el));

/* ── Tabs ── */
const tabBtns = document.querySelectorAll('.tabs__btn');
const tabPanels = document.querySelectorAll('.tab-panel');

tabBtns.forEach(btn => {
  btn.addEventListener('click', () => {
    const target = btn.dataset.tab;

    tabBtns.forEach(b => b.classList.remove('active'));
    tabPanels.forEach(p => p.classList.remove('active'));

    btn.classList.add('active');
    const panel = document.getElementById(`tab-${target}`);
    if (panel) panel.classList.add('active');
  });
});

/* ── Video Modal ── */
const videoModal   = document.getElementById('videoModal');
const heroVideo    = document.getElementById('heroVideo');
const openVideoBtn = document.getElementById('openVideo');
const closeVideoBtn= document.getElementById('closeVideo');
const modalBackdrop= document.getElementById('modalBackdrop');

function openModal() {
  videoModal.classList.add('open');
  document.body.style.overflow = 'hidden';
  if (heroVideo) heroVideo.play().catch(() => {});
}
function closeModal() {
  videoModal.classList.remove('open');
  document.body.style.overflow = '';
  if (heroVideo) { heroVideo.pause(); heroVideo.currentTime = 0; }
}

if (openVideoBtn) openVideoBtn.addEventListener('click', e => { e.preventDefault(); openModal(); });
if (closeVideoBtn) closeVideoBtn.addEventListener('click', closeModal);
if (modalBackdrop) modalBackdrop.addEventListener('click', closeModal);
document.addEventListener('keydown', e => {
  if (e.key === 'Escape') closeModal();
});

/* ── Contact Form Validation & Submit ── */
const contactForm  = document.getElementById('contactForm');
const formSuccess  = document.getElementById('formSuccess');

function validateField(el, errorId, message) {
  const errEl = document.getElementById(errorId);
  if (!el) return true;
  let valid = true;

  if (el.type === 'checkbox') {
    valid = el.checked;
  } else if (el.type === 'email') {
    valid = /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(el.value.trim());
  } else if (el.type === 'tel') {
    valid = el.value.replace(/\D/g, '').length >= 10;
  } else {
    valid = el.value.trim().length > 0;
  }

  if (!valid) {
    el.classList.add('invalid');
    if (errEl) errEl.textContent = message;
  } else {
    el.classList.remove('invalid');
    if (errEl) errEl.textContent = '';
  }
  return valid;
}

// Live validation
if (contactForm) {
  const liveFields = contactForm.querySelectorAll('input, textarea, select');
  liveFields.forEach(field => {
    field.addEventListener('blur', () => {
      if (field.required) {
        const errId = `err-${field.id}`;
        const messages = {
          orgName: 'Укажите название организации',
          contactPerson: 'Укажите контактное лицо',
          phone: 'Введите корректный номер телефона',
          email: 'Введите корректный e-mail',
          consent: 'Необходимо согласие на обработку данных',
        };
        validateField(field, errId, messages[field.id] || 'Поле обязательно для заполнения');
      }
    });
  });

  contactForm.addEventListener('submit', e => {
    e.preventDefault();

    const checks = [
      validateField(document.getElementById('orgName'),       'err-orgName',       'Укажите название организации'),
      validateField(document.getElementById('contactPerson'), 'err-contactPerson', 'Укажите контактное лицо'),
      validateField(document.getElementById('phone'),         'err-phone',         'Введите корректный номер телефона'),
      validateField(document.getElementById('email'),         'err-email',         'Введите корректный e-mail'),
      validateField(document.getElementById('consent'),       'err-consent',       'Необходимо согласие на обработку данных'),
    ];

    if (checks.every(Boolean)) {
      // Simulate form submission (replace with real fetch/AJAX as needed)
      const submitBtn = contactForm.querySelector('[type="submit"]');
      submitBtn.disabled = true;
      submitBtn.textContent = 'Отправляем...';

      setTimeout(() => {
        contactForm.reset();
        submitBtn.disabled = false;
        submitBtn.textContent = 'Отправить заявку';
        formSuccess.classList.add('visible');
        formSuccess.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
        setTimeout(() => formSuccess.classList.remove('visible'), 8000);
      }, 1000);
    } else {
      // Scroll to first error
      const firstInvalid = contactForm.querySelector('.invalid');
      if (firstInvalid) firstInvalid.scrollIntoView({ behavior: 'smooth', block: 'center' });
    }
  });
}

/* ── Phone mask ── */
const phoneInput = document.getElementById('phone');
if (phoneInput) {
  phoneInput.addEventListener('input', e => {
    let val = e.target.value.replace(/\D/g, '');
    if (val.startsWith('8')) val = '7' + val.slice(1);
    if (val.startsWith('7')) {
      val = val.slice(0, 11);
      let masked = '+7';
      if (val.length > 1) masked += ' (' + val.slice(1, 4);
      if (val.length >= 4) masked += ') ' + val.slice(4, 7);
      if (val.length >= 7) masked += '-' + val.slice(7, 9);
      if (val.length >= 9) masked += '-' + val.slice(9, 11);
      e.target.value = masked;
    }
  });
}

/* ── Active nav link highlight ── */
const sections = document.querySelectorAll('section[id]');
const navLinks  = document.querySelectorAll('.nav__links a');

function updateActiveNavLink() {
  let currentId = '';
  sections.forEach(sec => {
    const top = sec.getBoundingClientRect().top;
    if (top < 100) currentId = sec.id;
  });
  navLinks.forEach(link => {
    link.classList.toggle('nav-active', link.getAttribute('href') === `#${currentId}`);
  });
}
window.addEventListener('scroll', updateActiveNavLink, { passive: true });