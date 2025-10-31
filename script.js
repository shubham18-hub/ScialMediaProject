const api = {
  async post(path, body) {
    const r = await fetch(path, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body || {}),
    });
    if (!r.ok) throw new Error((await r.json()).error || r.statusText);
    return r.json();
  },
  async del(path, body) {
    const r = await fetch(path, {
      method: 'DELETE',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body || {}),
    });
    if (!r.ok) throw new Error((await r.json()).error || r.statusText);
    return r.json();
  },
  async get(path) {
    const r = await fetch(path);
    if (!r.ok) throw new Error((await r.json()).error || r.statusText);
    return r.json();
  },
};

let me = null;

function $(id) { return document.getElementById(id); }
function li(html, cls) { const el = document.createElement('li'); if (cls) el.className = cls; el.innerHTML = html; return el; }

function setAuth(authed) {
  $('auth').classList.toggle('hidden', authed);
  $('app').classList.toggle('hidden', !authed);
}

function show(view) {
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.toggle('active', b.dataset.view === view));
  document.querySelectorAll('.view').forEach(v => v.classList.toggle('hidden', v.id !== `view-${view}`));
}

function renderAttachments(container, atts = []) {
  if (!atts.length) return;
  const wrap = document.createElement('div'); wrap.className = 'media';
  atts.forEach(a => {
    if ((a.mime||'').startsWith('image/')) {
      const img = document.createElement('img'); img.src = a.url; img.alt = a.name || '';
      wrap.appendChild(img);
    } else if ((a.mime||'').startsWith('video/')) {
      const vid = document.createElement('video'); vid.src = a.url; vid.controls = true;
      wrap.appendChild(vid);
    } else {
      const link = document.createElement('a'); link.href = a.url; link.textContent = a.name || a.url; link.target = '_blank';
      wrap.appendChild(link);
    }
  });
  container.appendChild(wrap);
}

async function refresh() {
  if (!me) return;
  $('me-name').textContent = me.username;
  $('me-id').textContent = me.id;
  // Feed
  const feed = await api.get(`/api/feed?user_id=${me.id}`);
  const ul = $('feed'); ul.innerHTML = '';
  feed.forEach((p) => {
    const d = new Date(p.created_at * 1000).toLocaleString();
    const cls = p.priority ? 'priority' : '';
    const item = li(`<strong>@${p.author_name}</strong> <small>${d}</small><br/>${p.content}`, cls);
    ul.appendChild(item);
    renderAttachments(item, p.attachments);
  });
  // Messages
  const msgs = await api.get(`/api/messages?user_id=${me.id}`);
  const mUl = $('messages'); mUl.innerHTML = '';
  msgs.forEach((m) => {
    const d = new Date(m.timestamp * 1000).toLocaleString();
    const arrow = m.sender_id === me.id ? '→' : '←';
    const cls = m.priority ? 'priority' : '';
    const item = li(`${arrow} <small>${d}</small> ${m.content}`, cls);
    mUl.appendChild(item);
    renderAttachments(item, m.attachments);
  });
  // Notifications
  const notifs = await api.get(`/api/notifications?user_id=${me.id}`);
  const nUl = $('notifications'); nUl.innerHTML = '';
  notifs.forEach((n) => {
    const d = new Date(n.timestamp * 1000).toLocaleString();
    const dot = n.is_read ? '' : '● ';
    const cls = n.priority ? 'priority' : '';
    nUl.appendChild(li(`${dot}${n.content} <small>${d}</small>`, cls));
  });
  // Profile
  await loadProfile(me.id);
}

async function uploadSelected(inputId) {
  const input = $(inputId);
  if (!input || !input.files || !input.files.length) return [];
  const fd = new FormData();
  Array.from(input.files).forEach(f => fd.append('files', f));
  const r = await fetch('/api/upload', { method: 'POST', body: fd });
  if (!r.ok) throw new Error('upload failed');
  const data = await r.json();
  return data.files || [];
}

$('btn-register').onclick = async () => {
  $('auth-msg').textContent = '';
  try {
    const data = await api.post('/api/register', { username: $('reg-username').value, password: $('reg-password').value });
    $('auth-msg').textContent = `Registered @${data.username} (id=${data.id}). Now login.`;
  } catch (e) { $('auth-msg').textContent = e.message; }
};

$('btn-login').onclick = async () => {
  $('auth-msg').textContent = '';
  try {
    const data = await api.post('/api/login', { username: $('login-username').value, password: $('login-password').value });
    me = data; setAuth(true); show('feed'); await refresh();
  } catch (e) { $('auth-msg').textContent = e.message; }
};

$('btn-logout').onclick = () => { me = null; setAuth(false); };

document.querySelectorAll('.nav-btn').forEach(b => b.addEventListener('click', async (e) => {
  const view = e.target.dataset.view; show(view);
  if (view === 'profile') await loadProfile(me.id);
}));

$('btn-post').onclick = async () => {
  if (!me) return;
  const content = $('post-content').value.trim();
  if (!content) return;
  const attachments = await uploadSelected('post-files');
  await api.post('/api/posts', { author_id: me.id, content, attachments });
  $('post-content').value = '';
  $('post-files').value = '';
  await refresh();
};

$('btn-search').onclick = async () => {
  const q = encodeURIComponent($('search-q').value);
  const users = await api.get(`/api/users?search=${q}`);
  const ul = $('search-results'); ul.innerHTML = '';
  users.forEach((u) => {
    const item = li(`@${u.username} (id=${u.id}) <button data-id=\"${u.id}\" class=\"follow\">Follow</button> <button data-id=\"${u.id}\" class=\"cf\">Add CF</button> <button data-id=\"${u.id}\" class=\"unfollow\">Unfollow</button>`);
    ul.appendChild(item);
  });
  ul.querySelectorAll('button.follow').forEach((b) => b.addEventListener('click', async (e) => {
    const id = +e.target.getAttribute('data-id');
    if (id === me.id) return;
    await api.post('/api/follow', { follower_id: me.id, following_id: id });
    await refresh();
  }));
  ul.querySelectorAll('button.unfollow').forEach((b) => b.addEventListener('click', async (e) => {
    const id = +e.target.getAttribute('data-id');
    if (id === me.id) return;
    await api.post('/api/unfollow', { follower_id: me.id, following_id: id });
    await refresh();
  }));
  ul.querySelectorAll('button.cf').forEach((b) => b.addEventListener('click', async (e) => {
    const id = +e.target.getAttribute('data-id');
    if (id === me.id) return;
    await api.post('/api/close-friends', { user_id: me.id, friend_id: id });
    await refresh();
  }));
};

$('btn-send-msg').onclick = async () => {
  if (!me) return;
  const receiver = +$('msg-to').value;
  const content = $('msg-content').value.trim();
  if (!receiver || !content) return;
  const attachments = await uploadSelected('msg-files');
  await api.post('/api/messages', { sender_id: me.id, receiver_id: receiver, content, attachments });
  $('msg-content').value = '';
  $('msg-files').value = '';
  await refresh();
};

async function loadProfile(userId) {
  const user = await api.get(`/api/users/${userId}`);
  $('p-username').textContent = user.username;
  $('p-id').textContent = user.id;
  $('p-joined').textContent = new Date((user.createdAt || Date.now()/1000)*1000).toLocaleDateString();
  $('p-followers').textContent = user.followers;
  $('p-following').textContent = user.following;
  // lists
  const [followers, following, cfs, posts] = await Promise.all([
    api.get(`/api/followers?user_id=${userId}`),
    api.get(`/api/following?user_id=${userId}`),
    api.get(`/api/close-friends?user_id=${userId}`),
    api.get(`/api/posts?user_id=${userId}`),
  ]);
  const fl = $('p-followers-list'); fl.innerHTML = '';
  followers.forEach(u => fl.appendChild(li(`@${u.username} (id=${u.id})`)));
  const fg = $('p-following-list'); fg.innerHTML = '';
  following.forEach(u => fg.appendChild(li(`@${u.username} (id=${u.id}) <button data-id=\"${u.id}\" class=\"unfollow\">Unfollow</button> <button data-id=\"${u.id}\" class=\"cf\">Add CF</button>`)));
  const cf = $('p-cf-list'); cf.innerHTML = '';
  cfs.forEach(u => cf.appendChild(li(`⭐ @${u.username} (id=${u.id}) <button data-id=\"${u.id}\" class=\"rm-cf\">Remove</button>`)));
  const pUl = $('p-posts'); pUl.innerHTML = '';
  posts.forEach(p => {
    const item = li(`<strong>@${p.author_name}</strong> <small>${new Date(p.created_at*1000).toLocaleString()}</small><br/>${p.content}`);
    pUl.appendChild(item);
    renderAttachments(item, p.attachments);
  });
  fg.querySelectorAll('button.unfollow').forEach(b => b.addEventListener('click', async (e) => {
    const id = +e.target.getAttribute('data-id');
    await api.post('/api/unfollow', { follower_id: me.id, following_id: id });
    await loadProfile(userId); await refresh();
  }));
  fg.querySelectorAll('button.cf').forEach(b => b.addEventListener('click', async (e) => {
    const id = +e.target.getAttribute('data-id');
    await api.post('/api/close-friends', { user_id: me.id, friend_id: id });
    await loadProfile(userId); await refresh();
  }));
  cf.querySelectorAll('button.rm-cf').forEach(b => b.addEventListener('click', async (e) => {
    const id = +e.target.getAttribute('data-id');
    await api.del('/api/close-friends', { user_id: me.id, friend_id: id });
    await loadProfile(userId); await refresh();
  }));
}
