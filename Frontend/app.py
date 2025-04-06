from flask import Flask, render_template, request, send_file, Response
from supabase import create_client, Client
from supabase_config import url, key
from datetime import datetime
import pandas as pd
import requests
import io
import os

app = Flask(__name__)
supabase: Client = create_client(url, key)

@app.route('/', methods=['GET', 'POST'])
def index():
    patient_id = request.args.get('patient_id')
    if patient_id:
        response = supabase.table("patient_data").select("*").eq("patient_id", patient_id).execute()
    else:
        response = supabase.table("patient_data").select("*").execute()

    data = response.data or []
    for row in data:
        if 'timestamp' in row and row['timestamp']:
            row['timestamp'] = datetime.fromisoformat(row['timestamp'].replace('Z', '+00:00'))
    return render_template("index.html", data=data, patient_id=patient_id)

@app.route('/download_csv')
def download_csv():
    patient_id = request.args.get('patient_id')  # Get patient_id from query param

    if patient_id:
        response = supabase.table("patient_data").select("*").eq("patient_id", patient_id).execute()
    else:
        response = supabase.table("patient_data").select("*").execute()

    data = response.data or []

    # Optional: Format timestamp if needed
    for row in data:
        if 'timestamp' in row and row['timestamp']:
            row['timestamp'] = datetime.fromisoformat(row['timestamp'].replace('Z', '+00:00'))

    df = pd.DataFrame(data)
    csv = df.to_csv(index=False)
    return send_file(io.BytesIO(csv.encode()),
                     mimetype='text/csv',
                     as_attachment=True,
                     download_name=f"patient_data_{patient_id if patient_id else 'all'}.csv")


@app.route('/download_image')
def download_image():
    image_url = request.args.get('url')
    patient_id = request.args.get('patient_id', 'ecg_graph')
    
    if not image_url:
        return "Image URL missing", 400

    try:
        # Download the image from the provided URL
        img_response = requests.get(image_url)
        img_response.raise_for_status()

        # Return the image as a download
        return Response(
            img_response.content,
            mimetype='image/png',
            headers={
                "Content-Disposition": f"attachment; filename={patient_id}.png"
            }
        )
    except Exception as e:
        return f"Failed to download image: {str(e)}", 500


if __name__ == '__main__':
    app.run(debug=True)

