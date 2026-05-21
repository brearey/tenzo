// Плавная прокрутка и табы
document.addEventListener('DOMContentLoaded', function() {
    // Табы в блоке 3
    const tabButtons = document.querySelectorAll('.tab-button');
    const tabContents = document.querySelectorAll('.tab-content');

    if (tabButtons.length) {
        tabButtons.forEach(button => {
            button.addEventListener('click', () => {
                const tabId = button.getAttribute('data-tab');
                // удаляем активные классы
                tabButtons.forEach(btn => btn.classList.remove('active'));
                tabContents.forEach(content => content.classList.remove('active'));
                // добавляем активный
                button.classList.add('active');
                const activeContent = document.getElementById(tabId);
                if (activeContent) activeContent.classList.add('active');
            });
        });
    }

    // Функция скролла к секции
    function scrollToSection(sectionId) {
        const section = document.getElementById(sectionId);
        if (section) {
            section.scrollIntoView({ behavior: 'smooth', block: 'start' });
        } else {
            // если секции с таким id нет — скроллим к форме
            const formSection = document.getElementById('form-section');
            if (formSection) formSection.scrollIntoView({ behavior: 'smooth' });
        }
    }

    // Кнопка "Получить коммерческое предложение" — скролл к форме
    const getCPBtn = document.getElementById('getCPBtn');
    if (getCPBtn) {
        getCPBtn.addEventListener('click', () => {
            scrollToSection('form-section');
        });
    }

    // Кнопка "Узнать больше о проекте" — скролл к блоку "О проекте" (Блок 2)
    const learnMoreBtn = document.getElementById('learnMoreBtn');
    if (learnMoreBtn) {
        learnMoreBtn.addEventListener('click', () => {
            const aboutSection = document.querySelector('.about-project');
            if (aboutSection) {
                aboutSection.scrollIntoView({ behavior: 'smooth', block: 'start' });
            }
        });
    }

    // Кнопка "Рассчитать стоимость индивидуально" — скролл к форме
    const calculateBtn = document.getElementById('calculateBtn');
    if (calculateBtn) {
        calculateBtn.addEventListener('click', () => {
            scrollToSection('form-section');
        });
    }

    // Обработка отправки формы
    const form = document.getElementById('requestForm');
    if (form) {
        form.addEventListener('submit', function(e) {
            e.preventDefault();
            // Простейшая валидация
            const name = document.getElementById('name')?.value.trim();
            const institution = document.getElementById('institution')?.value.trim();
            const email = document.getElementById('email')?.value.trim();

            if (!name || !institution || !email) {
                alert('Пожалуйста, заполните обязательные поля: Имя, Учреждение и Email.');
                return;
            }

            if (!email.includes('@') || !email.includes('.')) {
                alert('Введите корректный email адрес.');
                return;
            }

            // Имитация отправки (в реальном проекте — fetch на сервер)
            alert('Спасибо! Ваша заявка принята. Мы свяжемся с вами в ближайшее время и откроем доступ на coreapp.ai.');
            form.reset();
        });
    }

    // Добавим анимацию появления блоков при скролле (простая)
    const animateOnScroll = function() {
        const elements = document.querySelectorAll('.card, .step, .package, .team-card, .value-grid div');
        elements.forEach(el => {
            const position = el.getBoundingClientRect().top;
            const screenPosition = window.innerHeight / 1.2;
            if (position < screenPosition) {
                el.style.opacity = '1';
                el.style.transform = 'translateY(0)';
            }
        });
    };

    // Установим начальные стили для анимации
    const animatedElements = document.querySelectorAll('.card, .step, .package, .team-card, .value-grid div');
    animatedElements.forEach(el => {
        el.style.opacity = '0';
        el.style.transform = 'translateY(20px)';
        el.style.transition = 'all 0.5s ease';
    });

    window.addEventListener('scroll', animateOnScroll);
    animateOnScroll(); // запуск при загрузке
});