export function formatFileSize(bytes: number): string {
  if (bytes === 0) return '0 B'
  const units = ['B', 'KB', 'MB', 'GB', 'TB']
  const i = Math.floor(Math.log(bytes) / Math.log(1024))
  const size = (bytes / Math.pow(1024, i)).toFixed(i === 0 ? 0 : 2)
  return `${size} ${units[i]}`
}

export function formatDate(dateStr: string): string {
  if (!dateStr) return ''
  const d = new Date(dateStr)
  const pad = (n: number) => n.toString().padStart(2, '0')
  return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(d.getMinutes())}`
}

export function fileTypeIcon(fileName: string): string {
  if (!fileName) return 'Document'
  const ext = fileName.split('.').pop()?.toLowerCase() || ''
  const map: Record<string, string> = {
    // images
    jpg: 'Picture', jpeg: 'Picture', png: 'Picture', gif: 'Picture', bmp: 'Picture', svg: 'Picture', webp: 'Picture',
    // video
    mp4: 'VideoCamera', avi: 'VideoCamera', mkv: 'VideoCamera', mov: 'VideoCamera', wmv: 'VideoCamera',
    // audio
    mp3: 'Headset', wav: 'Headset', flac: 'Headset', aac: 'Headset', ogg: 'Headset',
    // documents
    pdf: 'Document', doc: 'Document', docx: 'Document',
    xls: 'Grid', xlsx: 'Grid',
    ppt: 'DataBoard', pptx: 'DataBoard',
    txt: 'Notebook', md: 'Notebook',
    // code
    js: 'Files', ts: 'Files', vue: 'Files', html: 'Files', css: 'Files', json: 'Files', py: 'Files', go: 'Files', java: 'Files',
    // archive
    zip: 'Box', rar: 'Box', '7z': 'Box', tar: 'Box', gz: 'Box',
  }
  return map[ext] || 'Document'
}

export function fileTypeColor(fileName: string): string {
  if (!fileName) return '#909399'
  const ext = fileName.split('.').pop()?.toLowerCase() || ''
  const map: Record<string, string> = {
    // images - warm orange
    jpg: '#f59e0b', jpeg: '#f59e0b', png: '#f59e0b', gif: '#f59e0b', bmp: '#f59e0b', svg: '#f59e0b', webp: '#f59e0b',
    // video - rose red
    mp4: '#f43f5e', avi: '#f43f5e', mkv: '#f43f5e', mov: '#f43f5e', wmv: '#f43f5e',
    // audio - violet
    mp3: '#a855f7', wav: '#a855f7', flac: '#a855f7', aac: '#a855f7', ogg: '#a855f7',
    // pdf - red
    pdf: '#ef4444',
    // word - blue
    doc: '#356bfb', docx: '#356bfb',
    // excel - green
    xls: '#22c55e', xlsx: '#22c55e',
    // ppt - orange
    ppt: '#f97316', pptx: '#f97316',
    // text
    txt: '#64748b', md: '#64748b',
    // code - cyan
    js: '#06b6d4', ts: '#06b6d4', vue: '#06b6d4', html: '#06b6d4', css: '#06b6d4', json: '#06b6d4', py: '#06b6d4', go: '#06b6d4', java: '#06b6d4',
    // archive - amber
    zip: '#d97706', rar: '#d97706', '7z': '#d97706', tar: '#d97706', gz: '#d97706',
  }
  return map[ext] || '#909399'
}
