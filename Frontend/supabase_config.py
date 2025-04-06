from supabase import create_client

url = "https://wjqlervxbljhkscrteld.supabase.co"
key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6IndqcWxlcnZ4YmxqaGtzY3J0ZWxkIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDM4MjgxODUsImV4cCI6MjA1OTQwNDE4NX0.UJiOIJPtD4sY6Tgscur8RouPi-0jPIIYJuC18uerqyQ"

supabase = create_client(url, key)
