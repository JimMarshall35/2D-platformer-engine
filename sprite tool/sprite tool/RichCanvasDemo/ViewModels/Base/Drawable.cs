using RichCanvasDemo.Common;
using System;
using System.Windows;
using System.Windows.Input;
using System.Numerics;

namespace RichCanvasDemo.ViewModels.Base
{

    public abstract class Drawable : ObservableObject
    {
        public delegate void DrawablePropertyChangedHandler();

        public static event DrawablePropertyChangedHandler PropertiesChanged;
        private double _top;
        private double _left;
        private bool _isSelected;
        private bool _isSelectable = true;
        private bool _isDraggable = true;
        private double _width;
        private double _height;
        private VisualProperties _visualProperties;
        private bool _shouldBringIntoView;
        private Point _directionPoint;
        private RelayCommand<double> leftChangedCommand;
        private RelayCommand<double> topChangedCommand;
        private double _angle = 0;
        private bool _hasCustomBehavior;

        private string _drawablename;

        public string DrawableName
		{
            get => _drawablename;
            set
            {
                SetProperty(ref _drawablename, value);
                PropertiesChanged();
            }
		}

        public double Angle
        {
            get => _angle;
            set => SetProperty(ref _angle, value);
        }

        public double Top
        {
            get => _top;
            set
            {
                SetProperty(ref _top, value);
                PropertiesChanged();
            }
        }

        public double Left
        {
            get => _left;
            set
            {
                SetProperty(ref _left, value);
                PropertiesChanged();
            }
        }

        public bool IsSelected
        {
            get => _isSelected;
            set
            {
                SetProperty(ref _isSelected, value);
                OnIsSelectedChanged(value);
            }
        }

        public double Width
        {
            get => _width;
            set
            {
                SetProperty(ref _width, value);
                OnWidthUpdated();
                PropertiesChanged();
            }
        }

        public double Height
        {
            get => _height;
            set
            {
                SetProperty(ref _height, value);
                OnHeightUpdated();
                PropertiesChanged();
            }
        }

        public VisualProperties VisualProperties
        {
            get => _visualProperties;
            set => SetProperty(ref _visualProperties, value);
        }

        public bool IsSelectable
        {
            get => _isSelectable;
            set => SetProperty(ref _isSelectable, value);
        }

        public bool IsDraggable
        {
            get => _isDraggable;
            set => SetProperty(ref _isDraggable, value);
        }

        public bool HasCustomBehavior
        {
            get => _hasCustomBehavior;
            set => SetProperty(ref _hasCustomBehavior, value);
        }

        public bool ShouldBringIntoView
        {
            get => _shouldBringIntoView;
            set => SetProperty(ref _shouldBringIntoView, value);
        }

        public Point Scale
        {
            get => _directionPoint;
            set => SetProperty(ref _directionPoint, value);
        }

        public ICommand LeftChangedCommand => leftChangedCommand ??= new RelayCommand<double>(OnLeftChanged);

        public ICommand TopChangedCommand => topChangedCommand ??= new RelayCommand<double>(OnTopChanged);

        public Drawable()
        {
            VisualProperties = new VisualProperties();
            Scale = new Point(1, 1);
        }
        protected virtual void OnLeftChanged(double delta) { }

        protected virtual void OnTopChanged(double delta) { }

        protected virtual void OnWidthUpdated() { }

        protected virtual void OnHeightUpdated() { }

        protected virtual void OnIsSelectedChanged(bool value) { }

        public virtual void OnDrawingEnded(Action<object> callback = default) { }
    }
}
